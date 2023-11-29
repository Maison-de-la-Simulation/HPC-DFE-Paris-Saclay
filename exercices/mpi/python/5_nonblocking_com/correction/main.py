# ______________________________________________________________________________
#
# Exercice 5 : Communications non bloquantes point à point
#
# Cet exercice reprend le concept d'anneau utilisé dans l'exercice 4 
# mais au lieu d'utiliser la fonction sendrecv, nous allons utiliser 
# les communications non bloquantes isedn et irecv
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

# On détermine à qui on envoie `send_rank` et de qui on reçoit `recv_rank`:
tag = 0
send_rank = 0
recv_rank = 0
recv_message = 0

if rank == 0:
    send_rank = 1
    recv_rank = number_of_ranks - 1
elif rank == number_of_ranks - 1:
    send_rank = 0
    recv_rank = rank - 1
else:
    send_rank = rank + 1
    recv_rank = rank - 1

print("Le rang {} envoie le message {} au rang {}.".format(rank, rank, send_rank))

# On effectue l'envoi et la réception avec des communications non bloquantes

req_send = comm.isend(rank, dest=send_rank, tag=tag)
req_recv = comm.irecv(source=recv_rank, tag=tag)

req_send.wait()
recv_message = req_recv.wait()

# On affiche les résultats
print("Le rang {} a reçu le message {} du rang {}.".format(rank, recv_message, recv_rank))

# On finalise MPI
MPI.Finalize()