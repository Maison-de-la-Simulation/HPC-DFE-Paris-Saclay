# ______________________________________________________________________________
#
# Exercice 7 : Utilisation des communications collectives MPI_REDUCE
#
#
# Dans cet exercice, nous intégrons la fonction f(x) = sin(x) de
# manière numérique entre 0 et pi/2 et de manière parallèle
# On utilise une intégration par la méthode des rectangles.
# Le domaine est subdivisé suivant le nombre de processus MPI demandé.
#
#
#          delta
# 0         <>                                          pi/2
# |                 |                 |                 |
# |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
# |                 |                 |                 |
#  Premier domaine    deuxième domaine  troisième domaine
#  MPI
#  <--------------->
#    rank_length
#  |                |
# min              max
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

# __________________________________
# Paramètres globaux

discretization = 320000000

# _______________________________________________
# Command line arguments

import argparse

parser = argparse.ArgumentParser(description='Integration')
parser.add_argument('-N', type=int, help='Discretization', default=discretization)
args = parser.parse_args()

discretization = args.N

# _______________________________________________
# MPI init

# MPI init
MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# Chaque process récupère son numéro de rang dans le communicateur par défaut
rank = comm.Get_rank()

# _______________________________________________
# Summary
if rank == 0:
    print(" Integration: ")
    print("   - Number of ranks: ", number_of_ranks)
    print("   - Discretization: ", discretization)

# __________________________________
# Paramètres globaux

# Discrétisation par processus
discretization_per_rank = int(discretization / number_of_ranks)

# Taille d'un rang
rank_length = 0.5*np.pi / number_of_ranks

min   = rank * rank_length                            #Minimum local du rang
max   = (rank+1) * rank_length                        # Maximum local du rang
delta = rank_length / discretization_per_rank;         # Taille d'un rectangle

comm.Barrier()

time.sleep(0.5)

# __________________________________
# Intégration

local_integration = 0

start = MPI.Wtime()

# use numpy
x = np.arange(min+0.5*delta, max, delta)
local_integration = np.sum(np.sin(x)*delta)

# __________________________________
# Réduction MPI

integration = comm.reduce(local_integration, op=MPI.SUM, root=0)

end = MPI.Wtime()

# __________________________________
# Calcul du temps de calcul min, max et moyen

# On récupère le temps de calcul minimum avec un reduce
min_time = comm.reduce(end-start, op=MPI.MIN, root=0)
max_time = comm.reduce(end-start, op=MPI.MAX, root=0)
mean_time = comm.reduce(end-start, op=MPI.SUM, root=0)

# Affichage du résultat depuis le rang 0

if rank == 0:

    mean_time = mean_time / number_of_ranks

    print(" Time - min : {}, max :  {}, moyen:  {} secondes".format(min_time, max_time, mean_time))

# On finalise MPI
MPI.Finalize()

