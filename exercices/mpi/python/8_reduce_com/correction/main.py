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

# MPI init
MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# Chaque process récupère son numéro de rang dans le communicateur par défaut
rank = comm.Get_rank()

# __________________________________
# Paramètres globaux

discretization = 10000000

# Discrétisation par processus
discretization_per_rank = int(discretization / number_of_ranks)

# Taille d'un rang
rank_length = 0.5*np.pi / number_of_ranks

# __________________________________
# Paramètres locaux

min   = rank * rank_length                            #Minimum local du rang
max   = (rank+1) * rank_length                        # Maximum local du rang
delta = rank_length / discretization_per_rank;         # Taille d'un rectangle

# On affiche à l'écran quelques paramètres
print(" Le rang {} s'occupe de la portion comprise entre {} et {}".format(rank,min,max))

comm.Barrier()

time.sleep(0.5)

if (rank==0):
    print()

# __________________________________
# Intégration

local_integration = 0

start = MPI.Wtime()

# Normal loop
for i in range(discretization_per_rank):
    x = min + (i+0.5)*delta
    local_integration += np.sin(x)*delta

end_loop = MPI.Wtime() - start

start = MPI.Wtime()

# use numpy
x = np.arange(min+0.5*delta, max, delta)
local_integration = np.sum(np.sin(x)*delta)

end_numpy = MPI.Wtime() - start

print(" Le rang {} a pour intégration locale {} réalisée en {} ({}) secondes.".format(rank,local_integration, end_loop, end_numpy))

# __________________________________
# Réduction MPI

start = MPI.Wtime()

integration = comm.reduce(local_integration, op=MPI.SUM, root=0)

end = MPI.Wtime()

# Affichage du résultat depuis le rang 0

if rank == 0:

    print()
    print(" Le rang {} a pour résultat final {} réalisé en {} secondes".format(rank,integration, end-start))

# On finalise MPI
MPI.Finalize()

