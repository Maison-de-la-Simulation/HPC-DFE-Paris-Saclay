# ______________________________________________________________________________
#
# Exercice 2 : Premiers travaux en parallèle
#
# Dans cet exercice, vous apprendrez :
# - la notion de calcul en parallèle
# - à mesurer le temps
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
number_of_rank = comm.Get_size()

# Chaque process récupère son numéro de rang dans le communicateur par défaut
rank = comm.Get_rank()

# Chaque processus affiche ses informations :
print("Je suis le rang  {} sur {}".format(rank, number_of_rank))

# Nombre d'éléments :
N = 100000000

# Distribution de la charge (des éléments) entre les rangs
local_N = ???

# On va maintenant intialiser 3 tableaux numpy sur chaque rang de taille `local_N` de manière aléatoire
# Ici, il faut bien comprendre que chaque processus possède sa version de A, B et C.
???

# On récupère le temps avant :
start_timer = ???

# Chaque rang fait fait des calculs sur son tableau :
A = 2.9*B + 5.2*C + 265.7*B*C + B**2 + C**2

# On récupère le temps après :
stop_timer = ???

# Chaque rang va maintenant afficher le temps passé à faire ses calculs :
print("Le rang {} a fait ses calculs en {} s".format(rank, stop_timer - start_timer))

# On finalise MPI
MPI.Finalize()


