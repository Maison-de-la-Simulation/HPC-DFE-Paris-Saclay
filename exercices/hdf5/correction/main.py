# ______________________________________________________________________________
#
# Exercice HDF5 : correction
#
# Un tableau global de Nx x Ny valeurs est découpé en bandes de lignes,
# une bande par rang MPI. Chaque rang ne possède que sa bande, qu'il remplit
# avec son numéro de rang. Le tableau est ensuite écrit selon trois méthodes.
#
# Exécution : mpirun -np 4 python main.py --Nx 13 --Ny 8
# Vérification :
#   python ../check.py methode1_rang_*.h5
#   python ../check.py methode2.h5
#   python ../check.py methode3.h5
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


def print_time(comm, name, elapsed):
    """Affiche, depuis le rang 0, la durée elapsed du rang le plus lent."""
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

# Aucune communication : chaque rang écrit son propre fichier, avec en
# attributs sa position dans le tableau global pour pouvoir le reconstruire
with h5py.File(f"methode1_rang_{rank}.h5", "w") as f:
    dset = f.create_dataset("champ", data=local)
    dset.attrs["start"] = start
    dset.attrs["global_shape"] = (Nx, Ny)
    dset.attrs["number_of_ranks"] = number_of_ranks

end_time = MPI.Wtime()

print_time(comm, "Méthode 1 (un fichier par rang)", end_time - start_time)

# ______________________________________________________________________________
# Méthode 2 - Le rang 0 rassemble les bandes de tous les rangs et écrit seul
# le tableau global dans un seul fichier
#
# Fichier à produire : methode2.h5
# Vérification : python check.py methode2.h5

start_time = MPI.Wtime()

# Les bandes n'ont pas toutes la même taille : on utilise Gatherv.
# count : nombre de valeurs envoyées par chaque rang (n_local * Ny)
# displacement : position de la première valeur de chaque rang dans le
# tableau global, vu comme un tableau 1D (start * Ny)
count = [block_rows(Nx, number_of_ranks, r)[1] * Ny for r in range(number_of_ranks)]
displacement = [block_rows(Nx, number_of_ranks, r)[0] * Ny for r in range(number_of_ranks)]

if rank == 0:
    champ = np.empty((Nx, Ny), dtype=np.float64)
else:
    champ = None

comm.Gatherv(local, [champ, count, displacement, MPI.DOUBLE], root=0)

# Seul le rang 0 écrit le fichier
if rank == 0:
    with h5py.File("methode2.h5", "w") as f:
        dset = f.create_dataset("champ", data=champ)
        dset.attrs["number_of_ranks"] = number_of_ranks

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

if h5py.get_config().mpi:
    # Tous les rangs ouvrent le même fichier avec le pilote "mpio"
    with h5py.File("methode3.h5", "w", driver="mpio", comm=comm) as f:
        # Collectif : tous les rangs créent le dataset de taille globale
        dset = f.create_dataset("champ", (Nx, Ny), dtype="f8")
        dset.attrs["number_of_ranks"] = number_of_ranks

        # Chaque rang écrit sa bande, collectivement
        with dset.collective:
            dset[start:start + n_local, :] = local

    end_time = MPI.Wtime()

    print_time(comm, "Méthode 3 (HDF5 parallèle)", end_time - start_time)

elif rank == 0:
    print("Méthode 3 ignorée : h5py n'a pas été compilé avec MPI")
