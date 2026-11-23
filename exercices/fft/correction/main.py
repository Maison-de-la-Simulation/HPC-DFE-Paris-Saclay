# ______________________________________________________________________________
#
# Exercice FFT distribuée : correction
#
#     ∇²ψ = -ω     sur [0, Lx] x [0, Ly], conditions périodiques
#
# En Fourier : ψ_hat = ω_hat / k²,  avec k² = kx² + ky²
#
# Le programme résout l'équation par deux méthodes :
#   - Méthode 1 : tout rassembler sur le rang 0, FFT globale avec NumPy,
#                 puis redistribuer le résultat
#   - Méthode 2 : FFT distribuée avec mpi4py-fft
#
# Bonus : vitesse par dérivation spectrale, ux = ∂ψ/∂y,  uy = -∂ψ/∂x
#
# Exécution : mpirun -np 4 python main.py --Nx 256 --Ny 128
# ______________________________________________________________________________

import argparse

import numpy as np
from mpi4py import MPI
from mpi4py_fft import PFFT, newDistArray

# ______________________________________________________________________________
# Paramètres

parser = argparse.ArgumentParser(description="Équation de Poisson par FFT")
parser.add_argument("--Nx", type=int, default=256, help="nombre de points en x")
parser.add_argument("--Ny", type=int, default=128, help="nombre de points en y")
args = parser.parse_args()

Nx, Ny = args.Nx, args.Ny
Lx, Ly = 2.0, 1.0
dx, dy = Lx / Nx, Ly / Ny

# ______________________________________________________________________________
# MPI et découpage en bandes de lignes en x

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

if Nx % size != 0:
    if rank == 0:
        print(f"Erreur : Nx = {Nx} doit être divisible par le nombre de rangs ({size})")
    comm.Abort(1)

nx_local = Nx // size
i_start = rank * nx_local

x_local = np.arange(i_start, i_start + nx_local) * dx
y = np.arange(Ny) * dy
X, Y = np.meshgrid(x_local, y, indexing="ij")          # (nx_local, Ny)

# ______________________________________________________________________________
# Second membre ω et solution exacte ψ
#
# Pour ω = sin(a x) cos(b y), la solution de ∇²ψ = -ω est ψ = ω / (a² + b²).

a1, b1 = 2 * np.pi / Lx, 4 * np.pi / Ly
a2, b2 = 6 * np.pi / Lx, 2 * np.pi / Ly

omega_local = np.sin(a1 * X) * np.cos(b1 * Y) + 0.5 * np.cos(a2 * X) * np.sin(b2 * Y)
psi_exact_local = (np.sin(a1 * X) * np.cos(b1 * Y) / (a1**2 + b1**2)
                   + 0.5 * np.cos(a2 * X) * np.sin(b2 * Y) / (a2**2 + b2**2))

# Nombres d'onde globaux
kx = 2 * np.pi * np.fft.fftfreq(Nx, d=dx)
ky = 2 * np.pi * np.fft.fftfreq(Ny, d=dy)

if rank == 0:
    print(f"{size} rang(s), grille {Nx} x {Ny}")

# ______________________________________________________________________________
# Méthode 1 - Tout rassembler sur le rang 0
#
# Résultat attendu : psi_local, la bande (nx_local, Ny) de ψ de chaque rang

psi_local = None

comm.Barrier()
start_time = MPI.Wtime()

# Rassemblement des bandes de ω sur le rang 0
omega_parts = comm.gather(omega_local, root=0)

if rank == 0:
    # FFT 2D globale avec NumPy
    omega_global = np.vstack(omega_parts)                          # (Nx, Ny)
    KX, KY = np.meshgrid(kx, ky, indexing="ij")
    K2 = KX**2 + KY**2
    K2[0, 0] = 1.0                  # évite la division par 0 (mode moyen)
    psi_hat_global = np.fft.fft2(omega_global) / K2
    psi_global = np.real(np.fft.ifft2(psi_hat_global))

    # Découpage de ψ en bandes, une par rang
    psi_parts = [psi_global[r * nx_local:(r + 1) * nx_local, :] for r in range(size)]
else:
    psi_parts = None

# Redistribution des bandes de ψ
psi_local = comm.scatter(psi_parts, root=0)

comm.Barrier()
time_root = MPI.Wtime() - start_time

if psi_local is not None:
    error = comm.reduce(np.max(np.abs(psi_local - psi_exact_local)), op=MPI.MAX, root=0)
    if rank == 0:
        print("Méthode 1 (tout sur le rang 0)")
        print(f"  Erreur maximale sur psi : {error:.3e}")
        print(f"  Temps                   : {time_root * 1e3:.3f} ms")

# ______________________________________________________________________________
# Méthode 2 - FFT distribuée avec mpi4py-fft
#
# Préparation, hors de la mesure du temps : plan de FFT, tableaux distribués
# et nombres d'onde de la partie spectrale locale.

fft = PFFT(comm, (Nx, Ny), axes=(0, 1), dtype=np.float64)
omega_dist = newDistArray(fft, forward_output=False)
omega_hat = newDistArray(fft, forward_output=True)
psi_hat = newDistArray(fft, forward_output=True)
psi_dist = newDistArray(fft, forward_output=False)

# Le découpage de mpi4py-fft doit correspondre à nos bandes en x
if omega_dist.shape != omega_local.shape:
    if rank == 0:
        print(f"Erreur : découpage mpi4py-fft {omega_dist.shape} "
              f"différent du découpage du code {omega_local.shape}")
    comm.Abort(1)

# Données réelles : la dernière direction (y) n'a que Ny // 2 + 1 modes,
# donnés par rfftfreq
ky_r = 2 * np.pi * np.fft.rfftfreq(Ny, d=dy)
skx, sky = omega_hat.local_slice()
KX_loc, KY_loc = np.meshgrid(kx[skx], ky_r[sky], indexing="ij")
K2_loc = KX_loc**2 + KY_loc**2
if skx.start == 0 and sky.start == 0:
    K2_loc[0, 0] = 1.0

# Résolution, chronométrée
# Résultat attendu : psi_local, la bande (nx_local, Ny) de ψ de chaque rang

psi_local = None

comm.Barrier()
start_time = MPI.Wtime()

omega_dist[:] = omega_local
fft.forward(omega_dist, omega_hat)
psi_hat[:] = omega_hat / K2_loc
fft.backward(psi_hat, psi_dist)
psi_local = np.array(psi_dist, copy=True)

comm.Barrier()
time_distributed = MPI.Wtime() - start_time

if psi_local is not None:
    error = comm.reduce(np.max(np.abs(psi_local - psi_exact_local)), op=MPI.MAX, root=0)
    if rank == 0:
        print("Méthode 2 (FFT distribuée)")
        print(f"  Erreur maximale sur psi : {error:.3e}")
        print(f"  Temps                   : {time_distributed * 1e3:.3f} ms")

# ______________________________________________________________________________
# Bonus : vitesse par dérivation spectrale, ux = ∂ψ/∂y et uy = -∂ψ/∂x
#
# Dériver en x (resp. y) revient à multiplier par i kx (resp. i ky) en Fourier.
# Il faut les nombres d'onde de la partie spectrale locale (KX_loc, KY_loc),
# construits avec rfftfreq en y.

ux_hat = newDistArray(fft, forward_output=True)
uy_hat = newDistArray(fft, forward_output=True)
ux_dist = newDistArray(fft, forward_output=False)
uy_dist = newDistArray(fft, forward_output=False)

ux_hat[:] = 1j * KY_loc * psi_hat
uy_hat[:] = -1j * KX_loc * psi_hat
fft.backward(ux_hat, ux_dist)
fft.backward(uy_hat, uy_dist)

# Dérivées exactes de ψ
ux_exact = (-b1 * np.sin(a1 * X) * np.sin(b1 * Y) / (a1**2 + b1**2)
            + 0.5 * b2 * np.cos(a2 * X) * np.cos(b2 * Y) / (a2**2 + b2**2))
uy_exact = -(a1 * np.cos(a1 * X) * np.cos(b1 * Y) / (a1**2 + b1**2)
             - 0.5 * a2 * np.sin(a2 * X) * np.sin(b2 * Y) / (a2**2 + b2**2))

error_ux = comm.reduce(np.max(np.abs(ux_dist - ux_exact)), op=MPI.MAX, root=0)
error_uy = comm.reduce(np.max(np.abs(uy_dist - uy_exact)), op=MPI.MAX, root=0)

if rank == 0:
    print("Bonus (vitesse par dérivation spectrale)")
    print(f"  Erreur maximale sur ux  : {error_ux:.3e}")
    print(f"  Erreur maximale sur uy  : {error_uy:.3e}")
