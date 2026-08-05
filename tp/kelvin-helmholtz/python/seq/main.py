r"""
Kelvin-Helmholtz Instability — Sequential version
Vorticity-Streamfunction Formulation of 2D Navier-Stokes

  dω/dt + (u·∇)ω = ν ∇²ω
  ∇²ψ = -ω
  u_x = \frac{\partial \psi}{\partial x}, \quad u_y = -\frac{\partial \psi}{\partial y}

Domain is solved directly on the full grid, without MPI decomposition.

Numerical methods:
    - Poisson       : global FFT2 solve
    - x-derivatives : 2nd-order centred FD with periodic ghost cells
    - y-derivatives : 2nd-order centred FD with periodic wrap
    - Time          : explicit Euler

Run:
    python main.py
"""

import argparse
import os
import shutil
import numpy as np
import time

# ___________________________________________________________________________
# Global parameters

Nx, Ny = 256, 128
Lx, Ly = 2.0, 1.0
Re = 1000.0
dt = 1e-3
T_end = 20.0
output_period = 0.1
print_period = 1.0
output_dir = "diags"
clear_output = True
delta = 0.05
amp = 0.001

nu = 1.0 / Re

# ___________________________________________________________________________
# Parsing

parser = argparse.ArgumentParser(description="Kelvin-Helmholtz sequential solver")
parser.add_argument("--Nx", "--nx", dest="Nx", type=int, default=Nx,
                    help="Global number of grid points in the x direction")
parser.add_argument("--Ny", "--ny", dest="Ny", type=int, default=Ny,
                    help="Global number of grid points in the y direction")
parser.add_argument("--Lx", dest="Lx", type=float, default=Lx,
                    help="Domain length in x")
parser.add_argument("--Ly", dest="Ly", type=float, default=Ly,
                    help="Domain length in y")
parser.add_argument("--T_end", dest="T_end", type=float, default=T_end,
                    help="Final simulation time")
parser.add_argument("--dt", dest="dt", type=float, default=dt,
                    help="Time step")
args = parser.parse_args()

Nx = args.Nx
Ny = args.Ny
Lx = args.Lx
Ly = args.Ly
T_end = args.T_end
dt = args.dt

if Nx <= 0 or Ny <= 0:
    raise ValueError("Nx and Ny must be positive integers")

# ___________________________________________________________________________
# Internal parameters

n_steps     = int(T_end / dt)
save_every  = max(1, round(output_period / dt))
print_every = max(1, round(print_period / dt))
n_snapshots = n_steps // save_every

# Timers (seconds)
timer_advection = 0.0
timer_diffusion = 0.0
timer_euler = 0.0
timer_poisson = 0.0
timer_velocity = 0.0
timer_energy = 0.0
timer_snapshot_write = 0.0
energy_total = 0.0

# ___________________________________________________________________________
# Domain parameters

dx = Lx / Nx
dy = Ly / Ny

x = np.linspace(0, Lx, Nx, endpoint=False)
y = np.linspace(0, Ly, Ny, endpoint=False)

X, Y = np.meshgrid(x, y, indexing='ij')

# ___________________________________________________________________________
# Global wave numbers  (used by the FFT Poisson solver)

kx = 2.0 * np.pi * np.fft.fftfreq(Nx, d=dx)   # full x wave numbers
ky = 2.0 * np.pi * np.fft.fftfreq(Ny, d=dy)   # full y wave numbers

# Global K² for Poisson solve on rank 0
KX_global, KY_global = np.meshgrid(kx, ky, indexing='ij')
K2_global = KX_global**2 + KY_global**2
K2_global[0, 0] = 1.0   # avoid division by zero for the mean mode


# ___________________________________________________________________________
# Spatial operators

def d_dx_fd(fg):
    """∂f/∂x — 2nd-order centred FD from an already-extended ghosted field.
    Always uses nearest neighbours f[i+1], f[i-1]
    """
    return (np.roll(fg, -1, axis=0) - np.roll(fg, 1, axis=0)) / (2.0 * dx)


def d_dy_fd(f):
    """∂f/∂y — 2nd-order centred FD, periodic BCs (y not decomposed, use np.roll).
    f : (nx_local, Ny)  →  (nx_local, Ny)
    """
    return (np.roll(f, -1, axis=1) - np.roll(f, 1, axis=1)) / (2.0 * dy)

# ___________________________________________________________________________
# Right hand side terms

def advection_term(ux, uy, omega):
    """Compute the advective term A = -(ux·∇)ω from the ghost-extended omega field."""
    domega_dx = d_dx_fd(omega)
    domega_dy = d_dy_fd(omega)
    return -(ux * domega_dx + uy * domega_dy)


def diffusion_term(omega):
    """Compute the diffusive term D = ν∇²ω from the ghost-extended omega field."""
    d2x = (np.roll(omega, -1, axis=0) - 2.0 * omega + np.roll(omega, 1, axis=0)) / dx**2
    d2y = (np.roll(omega, -1, axis=1) - 2.0 * omega + np.roll(omega, 1, axis=1)) / dy**2
    return nu * (d2x + d2y)

# ___________________________________________________________________________
# Poisson solver

def solve_poisson(omega):
    """Solve ∇²ψ = -ω in Fourier space. Returns ψ in physical space."""
    omega_hat = np.fft.fft2(omega)
    psi_hat = omega_hat / K2_global
    psi = np.real(np.fft.ifft2(psi_hat))
    return psi

def velocity_from_psi(psi):
    """Return (ux, uy) from a ghost-extended psi field."""
    ux = d_dy_fd(psi)
    uy = -d_dx_fd(psi)
    return ux, uy

def compute_velocity(omega):
    """Return (ux, uy) from ψ via FD derivatives.
      ux =  ∂ψ/∂y  — FD
      uy = -∂ψ/∂x  — FD
    """
    psi = solve_poisson(omega)
    return velocity_from_psi(psi)

# ___________________________________________________________________________
# Energy

def total_energy(ux, uy):
    """Compute the total kinetic energy"""
    return dx * dy * np.sum(0.5 * (ux**2 + uy**2))

# ___________________________________________________________________________
# Initial condition

def initial_vorticity(X, Y, Ly, delta, amp):
    omega  = (1.0 / delta) * np.cosh((Y - 0.25 * Ly) / delta)**-2
    omega -= (1.0 / delta) * np.cosh((Y - 0.75 * Ly) / delta)**-2
    omega += amp * np.sin(2.0 * np.pi * X / Lx)
    return omega

omega = initial_vorticity(X, Y, Ly, delta, amp)

# ___________________________________________________________________________
# I/O : save snapshots and prepare output directory

def save_snapshot(output_dir, step, t, omega, ux, uy):
    """Gather local fields on rank 0 and write a single global snapshot file."""

    os.makedirs(output_dir, exist_ok=True)
    path = os.path.join(output_dir, f"snapshot_{step:06d}.npz")
    np.savez(
        path,
        omega=omega,
        ux=ux,
        uy=uy,
        time=np.float64(t),
        dx=np.float64(dx),
        dy=np.float64(dy),
        Lx=np.float64(Lx),
        Ly=np.float64(Ly),
        nx=np.int32(Nx),
        ny=np.int32(Ny),
    )

def prepare_output_dir(path, clear=False):
    """Prepare output directory."""
    if clear and os.path.isdir(path):
        shutil.rmtree(path)
    os.makedirs(path, exist_ok=True)

# ___________________________________________________________________________
# Parameters summary

sep = "-" * 56
print(sep)
print(f"  Kelvin-Helmholtz simulation")
print(sep)
print(f" 📋 Simulation parameters:")
print(f"  - Grid          : {Nx} x {Ny}  (dx={dx:.4e}, dy={dy:.4e})")
print(f"  - Domain        : Lx={Lx},  Ly={Ly}")
print(f"  - Reynolds      : {Re}")
print(f"  - Viscosity     : nu = {nu:.4e}")
print(f"  - dt            : {dt:.4e}")
print(f"  - T_end         : {T_end}")
print(f"  - Steps         : {n_steps}")
print(f"  - Shear delta   : {delta}")
print(f"  - Perturbation  : {amp}")
dt_cfl = min(dx, dy)**2 / (2.0 * nu)
print(f"  - Output dir    : {output_dir}/")
print(f"  - Clear output  : {clear_output}")
print(f"  - Output period : {output_period}  ({save_every} steps)")
print(f"  - Snapshots     : ~{n_snapshots} files (written by rank 0)")
print(sep)

prepare_output_dir(output_dir, clear_output)

# ___________________________________________________________________________
#
# Main time loop
# ___________________________________________________________________________

t = 0.0

psi = solve_poisson(omega)
ux, uy = velocity_from_psi(psi)

timer_loop = time.time()

print(f" ⚙️  Starting main time loop")
print(sep)
header = f"{'step':>8} {'energy':>16}"
print(header)
print("-" * len(header))

for i in range(n_steps):

    # 2. Compute omega derivatives for the advection term

    t0 = time.time()
    A = advection_term(ux, uy, omega)
    timer_advection += time.time() - t0

    # 3. Compute the diffusion term of the vorticity equation

    t0 = time.time()
    D = diffusion_term(omega)
    timer_diffusion += time.time() - t0

    # 4. Update omega with explicit Euler step

    t0 = time.time()
    omega = omega + dt * (A + D)
    timer_euler += time.time() - t0

    t += dt

    # 5. Update Psi by solving Poisson

    t0 = time.time()
    psi = solve_poisson(omega)
    timer_poisson += time.time() - t0

    # 6. Update velocity from Psi

    t0 = time.time()
    ux, uy = velocity_from_psi(psi)
    timer_velocity += time.time() - t0

    # 7. Compute and track the kinetic energy
    t0 = time.time()
    energy = total_energy(ux, uy)
    timer_energy += time.time() - t0

    # 8. Save snapshot (rank 0 writes global file)

    if i % save_every == 0:
        t0 = time.time()
        save_snapshot(output_dir, i, t, omega, ux, uy)
        timer_snapshot_write += time.time() - t0

    # 9. Print progress (rank 0 only)

    if i % print_every == 0:
        print(f"{i:>8d} {energy:>16.6e}")

timer_loop = time.time() - timer_loop

# ___________________________________________________________________________
#
# Timing summary
# ___________________________________________________________________________

timer_specs = [
    ("Advection term", timer_advection),
    ("Diffusion term", timer_diffusion),
    ("Euler compute", timer_euler),
    ("Poisson solver", timer_poisson),
    ("Velocity computation", timer_velocity),
    ("Energy computation", timer_energy),
    ("snapshot write total", timer_snapshot_write),
    ("loop total", timer_loop),
]

timer_stats = []
for timer_name, timer_value in timer_specs:
    percentage = timer_value / timer_loop * 100.0
    timer_stats.append((timer_name, timer_value, percentage))

print(sep)
print(" ⏱️  Timing summary (seconds across ranks):")
header = f"{' nom du timer':<26} {'temps ':>12} {'pourcentage':>12}"
print(header)
print("-" * len(header))
for timer_name, timer_value, percentage in timer_stats:
    print(f" {timer_name:<26} {timer_value:12.3f} {percentage:12.3f}")
