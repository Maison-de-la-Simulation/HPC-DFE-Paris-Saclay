# ______________________________________________________________________________
#
# Exercice 3 : communication bloquante point à point
#
# Ce second programme présente le fonctionnement des communications
# bloquantes send/recv
# Dans ce code, le processus MPI de rang 1 envoie un message
# au processus de rang 2.
#
# ______________________________________________________________________________

import mpi4py

# En Python, l'initialisation de MPI est automatique mais ce n'est pas le cas des autres langages comme C, C++ ou Fortran, on désactive donc l'initialisation automatique pour l'exemple :
mpi4py.rc.initialize = False  
mpi4py.rc.finalize = False

# En Python, la bibliothèque MPI se récupère dans le paquet mpi4py
from mpi4py import MPI
import numpy as np

# MPI init
MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# Chaque process récupère son numéro de rang dans le communicateur par défaut
rank = comm.Get_rank()

# Tous les processus ont la variable message initialisée à 0

message = 0
tag = 0

# Envoi du message du rang 1 vers le rang 2

if rank == 1:

    message = 42
    comm.send(message, dest=2, tag=tag)

    print("Le rang {} envoie le message {} au rang 2".format(rank, message))

# Reception du message du rang 2 venant du rang 1

elif rank == 2:

    message = comm.recv(source=1, tag=tag)
    print("Le rang {} a reçu le message {} du rang 1".format(rank, message))


# On finalise MPI
MPI.Finalize()
