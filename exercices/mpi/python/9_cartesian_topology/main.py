# ______________________________________________________________________________
#
# Exercice : Création d'une topologie cartésienne 2D
#
# Dans cet exercice, nous allons créer une topologie cartésienne 2D
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

# Création d'un communicateur cartésien 2D
# On définit le nombre de dimensions
ndims = 2

# On définit le nombre de processus par dimension
dims = [4, 5]

# On définit si les dimensions sont périodiques ou non
periods = [False, False]

# On crée le communicateur cartésien
???

# On récupère le rang dans le communicateur cartésien
???

# On récupère les coordonnées du rang dans le communicateur cartésien
???

# On récupère le rang du processus voisin dans la dimension 0
# avec un décalage de +1
left_rank = ???
right_rank = ???

# On récupère le rang du processus voisin dans la dimension 1
# avec un décalage de +1
up_rank = ???
down_rank = ???

# Chaque rang affiche ses informations
print("Je suis le rang {} dans le communicateur global et le rang {} dans le communicateur cartésien avec pour coordonnées {} et voisins : haut = {}, bas = {}, gauche = {}, droite = {}.".format(rank, cart_rank, cart_coords, up_rank, down_rank, left_rank, right_rank))

# Le rang 0 print la matrice des rangs en utilisant les coordonnées des rangs

# On récupère le rang 0 dans le communicateur cartésien
if cart_rank == 0:

    # On fabrique maintenant une matrice `ranks_matrix` qui a la même taille de la topologie cartésienne 2d 
    ???

    # Boucle sur les rangs pour remplir la matrice
    ???

    # On affiche la matrice finale
    print("Matrice des rangs : \n{}".format(ranks_matrix))

# Finalize MPI
MPI.Finalize()

