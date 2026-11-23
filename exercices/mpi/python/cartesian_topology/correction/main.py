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
cart_comm = comm.Create_cart(dims, periods, reorder=True)

# On récupère le rang dans le communicateur cartésien
cart_rank = cart_comm.Get_rank()

# On récupère les coordonnées du rang dans le communicateur cartésien
cart_coords = cart_comm.Get_coords(cart_rank)

# On récupère le rang du processus voisin dans la dimension 0
# avec un décalage de +1
left_rank = cart_comm.Shift(0, 1)[0]
right_rank = cart_comm.Shift(0, 1)[1]

# On récupère le rang du processus voisin dans la dimension 1
# avec un décalage de +1
up_rank = cart_comm.Shift(1, 1)[0]
down_rank = cart_comm.Shift(1, 1)[1]

# Chaque rang affiche ses informations
print("Je suis le rang {} dans le communicateur global et le rang {} dans le communicateur cartésien avec pour coordonnées {} et voisins : haut = {}, bas = {}, gauche = {}, droite = {}.".format(rank, cart_rank, cart_coords, up_rank, down_rank, left_rank, right_rank))

# Le rang 0 print la matrice des rangs en utilisant les coordonnées des rangs

# On récupère le rang 0 dans le communicateur cartésien
if cart_rank == 0:

    # On crée une matrice de taille dims
    ranks_matrix = np.zeros(dims, dtype=np.int32)

    # Pour chaque rang, on récupère ses coordonnées et on les place dans la matrice
    for rank in range(number_of_ranks):

        # On récupère les coordonnées du rang
        coords = cart_comm.Get_coords(rank)

        # On place le rang dans la matrice
        ranks_matrix[coords[0], coords[1]] = rank

    # On affiche la matrice finale
    print("Matrice des rangs : \n{}".format(ranks_matrix))

# Finalize MPI
MPI.Finalize()

