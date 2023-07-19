# ______________________________________________________________________________
#
# Exercice 6 : Communication de tableaux
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

# ________________________________________________
# Envoi d'un tableau d'entiers via des 
# communications bloquantes du rang 0 au rang 3 :

tag = 0
send_rank = 0
recv_rank = 3

if rank == send_rank:

    send_buffer = np.arange(5, dtype=np.int32)
    
    ???

    print("Le rang {} envoie le message {} au rang {}".format(rank, send_buffer, recv_rank))

elif rank == recv_rank:

    ???

    print("Le rang {} a reçu le message {} du rang {}".format(rank, recv_buffer, send_rank))

comm.Barrier()

# ________________________________________________
# Envoi d'un tableau de float via des 
# communications non bloquantes du rang 2 au rang 1 :

tag = 0
send_rank = 2
recv_rank = 1

if rank == send_rank:

    send_buffer = np.arange(6, dtype=np.float32)

    ???

    print("Le rang {} envoie le message {} au rang {}".format(rank, send_buffer, recv_rank))

elif rank == recv_rank:

    ???
    
    print("Le rang {} a reçu le message {} du rang {}".format(rank, recv_buffer, send_rank))

# On finalise MPI
MPI.Finalize()
