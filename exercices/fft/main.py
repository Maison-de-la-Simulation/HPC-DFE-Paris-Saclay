# ______________________________________________________________________________
#
# Exercice FFT distribuée : résolution de l'équation de Poisson
#
#     ∇²ψ = -ω     sur [0, Lx] x [0, Ly], conditions périodiques
#
# En Fourier : ψ_hat = ω_hat / k²,  avec k² = kx² + ky²
#
# Le domaine est découpé en bandes de lignes en x, une par rang MPI.
# Le second membre ω est choisi pour que la solution exacte ψ soit connue :
# on peut donc mesurer l'erreur de chaque méthode.
#
# Le programme résout l'équation par deux méthodes, à compléter :
#   - Méthode 1 : tout rassembler sur le rang 0, FFT globale avec NumPy,
#                 puis redistribuer le résultat
#   - Méthode 2 : FFT distribuée avec mpi4py-fft
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

# À compléter

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

# À compléter

# Résolution, chronométrée
# Résultat attendu : psi_local, la bande (nx_local, Ny) de ψ de chaque rang

psi_local = None

comm.Barrier()
start_time = MPI.Wtime()

# À compléter

comm.Barrier()
time_distributed = MPI.Wtime() - start_time

if psi_local is not None:
    error = comm.reduce(np.max(np.abs(psi_local - psi_exact_local)), op=MPI.MAX, root=0)
    if rank == 0:
        print("Méthode 2 (FFT distribuée)")
        print(f"  Erreur maximale sur psi : {error:.3e}")
        print(f"  Temps                   : {time_distributed * 1e3:.3f} ms")
