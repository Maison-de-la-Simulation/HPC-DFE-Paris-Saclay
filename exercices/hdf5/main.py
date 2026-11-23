# ______________________________________________________________________________
#
# Exercice HDF5 : code de départ
#
# Un tableau global de Nx x Ny valeurs est découpé en bandes de lignes,
# une bande par rang MPI. Chaque rang ne possède que sa bande, qu'il remplit
# avec son numéro de rang.
#
# Ce code ne sauvegarde encore rien : c'est à vous de compléter les trois
# méthodes d'écriture du tableau dans des fichiers HDF5 (voir README.md).
#
# Exécution : mpirun -np 4 python main.py --Nx 13 --Ny 8
# ______________________________________________________________________________

import argparse

import h5py
import numpy as np
from mpi4py import MPI


def block_rows(Nx, number_of_ranks, rank):
    """Renvoie (start, n_local) : première ligne et nombre de lignes du rang.

    Les lignes sont réparties en blocs contigus ; les premiers rangs prennent
    une ligne de plus quand Nx n'est pas divisible par number_of_ranks.
    """
    n_local = Nx // number_of_ranks + (1 if rank < Nx % number_of_ranks else 0)
    start = rank * (Nx // number_of_ranks) + min(rank, Nx % number_of_ranks)
    return start, n_local


def print_time(comm, name, start_time):
    """Affiche, depuis le rang 0, le temps du rang le plus lent depuis start_time."""
    elapsed = MPI.Wtime() - start_time
    max_elapsed = comm.reduce(elapsed, op=MPI.MAX, root=0)
    if comm.Get_rank() == 0:
        print(f"{name} : {max_elapsed:.4f} s")


# ______________________________________________________________________________
# Paramètres

parser = argparse.ArgumentParser(description="Tableau réparti en bandes de lignes")
parser.add_argument("--Nx", type=int, default=12, help="nombre de lignes du tableau global")
parser.add_argument("--Ny", type=int, default=8, help="nombre de colonnes du tableau global")
args = parser.parse_args()
Nx, Ny = args.Nx, args.Ny

# ______________________________________________________________________________
# MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
number_of_ranks = comm.Get_size()

# ______________________________________________________________________________
# Chaque rang construit uniquement sa bande du tableau global

start, n_local = block_rows(Nx, number_of_ranks, rank)
local = np.full((n_local, Ny), float(rank))

print(f"Rang {rank} : lignes {start} à {start + n_local - 1} "
      f"({n_local} x {Ny} valeurs)")

# ______________________________________________________________________________
# Méthode 1 - Chaque rang écrit sa bande dans un fichier séparé
#
# Fichiers à produire : methode1_rang_<rang>.h5
# Vérification : python check.py methode1_rang_*.h5

start_time = MPI.Wtime()

# À compléter

end_time = MPI.Wtime()

print_time(comm, "Méthode 1 (un fichier par rang)", end_time - start_time)

# ______________________________________________________________________________
# Méthode 2 - Le rang 0 rassemble les bandes de tous les rangs et écrit seul
# le tableau global dans un seul fichier
#
# Fichier à produire : methode2.h5
# Vérification : python check.py methode2.h5

start_time = MPI.Wtime()

# À compléter

end_time = MPI.Wtime()

print_time(comm, "Méthode 2 (tout sur le rang 0)", end_time - start_time)

# ______________________________________________________________________________
# Méthode 3 - Écriture parallèle dans un seul fichier HDF5 (MPI-IO)
#
# Nécessite h5py compilé avec MPI : h5py.get_config().mpi doit valoir True
#
# Fichier à produire : methode3.h5
# Vérification : python check.py methode3.h5

start_time = MPI.Wtime()

# À compléter

end_time = MPI.Wtime()

print_time(comm, "Méthode 3 (HDF5 parallèle)", end_time - start_time)
