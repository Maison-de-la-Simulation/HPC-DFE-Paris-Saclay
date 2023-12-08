# ______________________________________________________________________________
#
# Exercice : Utilisation des communications collectives MPI_Gather
#
#
# Dans cet exercice, chaque rang MPI alloue et initialise un tableau d'une taille
# déterminée de manière aléatoire.
# Le but est de rapatrier chaque morceau de tableau vers le rang 0.
#
# ______________________________________________________________________________

import mpi4py

# En Python, l'initialisation de MPI est automatique mais ce n'est pas le cas des autres langages comme C, C++ ou Fortran, on désactive donc l'initialisation automatique pour l'exemple :
mpi4py.rc.initialize = False  
mpi4py.rc.finalize = False

# En Python, la bibliothèque MPI se récupère dans le paquet mpi4py
from mpi4py import MPI
import numpy as np
import time

# MPI init
MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# Chaque process récupère son numéro de rang dans le communicateur par défaut
rank = comm.Get_rank()

# On définit la taille des tableaux à envoyer
size = 100

# On alloue un tableau de taille size
tab = np.zeros(size, dtype=np.float64)

# On initialise le tableau avec le numéro de rang
tab[:] = rank

# On ramène maintenant sur le rang 0 tous les tableaux locaux dans un tableau global
# en suivant l'ordre des rangs

# Taille du tableau global
global_size = size * number_of_ranks

# Allocation d'un tableau global sur le rang 0
if rank == 0:
    global_tab = np.zeros(global_size, dtype=np.float64)
else:
    global_tab = None

# On récupère les tableaux locaux sur le rang 0
comm.Gather(tab, global_tab, root=0)

# On affiche le tableau global sur le rang 0
if rank == 0:

    # Vérification de la somme
    sum_check = np.sum( np.arange(0, number_of_ranks, 1, dtype=np.float64)*size )

    print("Somme théorique du tableau global : {}".format(sum_check))
    print("Somme calculée du tableau global : {}".format(np.sum(global_tab)))

    print("Tableau global :")
    print(global_tab)

# On finalise MPI
MPI.Finalize()