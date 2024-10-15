# ____________________________________________________________________
#
# SHALLOW WATER 1D
# This program simulates the conservative 1d shallow water equations
#
# Description of the 1d structured discretized domain:
#
#  N is the number of points
#  |
#  |    `N - 1` is therefore the number of cells
#  |    |
#  v    v
#
#  |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|

#  <--> dx is the cell length
#
#  < -------- `length`is the domain length ----------------------->
# ____________________________________________________________________

# _____________________________________
#
# Libraries
# _____________________________________

import numpy as np
import matplotlib.pyplot as plt
import struct
import os
import time
import sys

import mpi4py

# En Python, l'initialisation de MPI est automatique mais ce n'est pas le cas des autres langages comme C, C++ ou Fortran, on désactive donc l'initialisation automatique pour l'exemple :
mpi4py.rc.initialize = False
mpi4py.rc.finalize = False

# En Python, la bibliothèque MPI se récupère dans le paquet mpi4py
from mpi4py import MPI

# _____________________________________
#
# Input parameters
# _____________________________________

# Space (L)
L = 5.0

# Number of points (N)
N = 10240

# Number of iterations
iterations = 10000

# gravity
g = 9.8

# Terminal output period
print_period = 1000

# Disk output period
output_period = 100

# Matplotlib display period
# if 0, the matplotlib figure display is disabled
matplotlib_period = 0
# Display time for each figure (careful, it stops the computation)
matplotlib_pause_duration = 0.1

# Lecture en ligne de commande de L avec le paramètre -L, de N avec le paramètre -N et de iterations avec le paramètre -i, et de output_period avec le paramètre -o, de print_period avec le paramètre -p, de matplotlib_period avec le paramètre -mp et de matplotlib_pause_duration avec le paramètre -md

for i in range(1, len(sys.argv)):
    if sys.argv[i] == "-L":
        L = float(sys.argv[i + 1])
    if sys.argv[i] == "-N":
        N = int(sys.argv[i + 1])
    if sys.argv[i] == "-i":
        iterations = int(sys.argv[i + 1])
    if sys.argv[i] == "-o":
        output_period = int(sys.argv[i + 1])
    if sys.argv[i] == "-p":
        print_period = int(sys.argv[i + 1])
    if sys.argv[i] == "-mp":
        matplotlib_period = int(sys.argv[i + 1])
    if sys.argv[i] == "-md":
        matplotlib_pause_duration = float(sys.argv[i + 1])

# _____________________________________
#
# MPI initialization
# _____________________________________

MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# On récupère le rang du processus courant
rank = comm.Get_rank()

# Dimensions
dims = [number_of_ranks]

# Création d'un communicateur cartésien 1D
ndims = 1

# On définit si les dimensions sont périodiques ou non
periods = [False]

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

# _____________________________________
#
# Initialization
# _____________________________________

dx = L / (N - 1)
invdx = 1 / dx

dt = 0.1 * dx
duration = iterations * dt

# Taille du sous-domaine (sans les points fantômes)
decomposition_size = N // number_of_ranks

# On ajoute les points fantômes pour les rangs qui ne sont pas sur les extrémités
left_ghost_cell = 0
if left_rank != MPI.PROC_NULL:
    left_ghost_cell = 1

right_ghost_cell = 0
if right_rank != MPI.PROC_NULL:
    right_ghost_cell = 1

# Taille du sous-domaine (avec les points fantômes)
local_size = (N) // number_of_ranks + left_ghost_cell + right_ghost_cell

# On détermine le début et la fin du sous-domaine local
local_start = decomposition_size * rank - left_ghost_cell
local_end = decomposition_size * (rank + 1) + right_ghost_cell - 1

# initial heigh of the water: the dam break
height = np.zeros(local_size)
for ix in range(local_size):
    x_value = dx * (local_start + ix)
    if x_value < 0.4 * L:
        height[ix] = 2.0
    elif x_value > 0.6 * L:
        height[ix] = 1.0
    else:
        height[ix] = 1 + 0.5 * (1.0 + np.cos(np.pi * (x_value - 0.4 * L) / (0.2 * L)))

# initial heigh of the water: Gaussian
# height  = np.zeros(N)
# for ix, x_value in enumerate(x):
#   if (x_value < 0.4 * L):
#     height[ix] = 1.
#   elif (x_value > 0.6 * L):
#     height[ix] = 1.
#   else:
#     height[ix] = 1. + np.exp( - (x_value - 0.5*L)**2 / (0.1 * L)**2)

# water flow = ordinary velocity u(x,t) times water heigth h(x,t)
q = np.zeros(local_size)

# height at mid-point and half time steps
height_m = np.zeros(local_size - 1)

# water flow at mid-point and half time steps
q_m = np.zeros(local_size - 1)

# Adjust the boundary conditions for init
if left_rank == MPI.PROC_NULL:
    height[0] = height[1]

if right_rank == MPI.PROC_NULL:
    height[local_size - 1] = height[local_size - 2]

# Maximal height value
local_max_height = np.max(height)
max_height = comm.reduce(local_max_height, op=MPI.MAX, root=0)

# Sum height value (we average at the midle of the cell)
local_sum_height = (
    np.sum(
        height[left_ghost_cell : local_size - 1]
        + height[left_ghost_cell + 1 : local_size]
    )
    * 0.5
)
sum_height = comm.reduce(local_sum_height, op=MPI.SUM, root=0)

if rank == 0:

    # Average height value
    average_height = sum_height / (N - 1)

    # Water quantity value
    water_quantity = sum_height * dx

# Output directory management
if (output_period > 0) and (rank == 0):
    if not os.path.exists("diags"):
        os.mkdir("diags")

# _____________________________________
#
# Terminal summary
# _____________________________________

if comm.rank == 0:

    print(" ------------------------------------------------------------------------- ")
    print(" SHALLOW WATER 1D")
    print(" ------------------------------------------------------------------------- ")
    print("")
    print("  - total L: {}".format(L))
    print("  - total N: {}".format(N))
    print("  - decomposition N: {}".format(decomposition_size))
    print("  - dx: {}".format(dx))
    print("  - duration: {}".format(duration))
    print("  - iterations: {}".format(iterations))
    print("  - dt: {}".format(dt))
    print("  - print period: {}".format(print_period))
    print("  - max height: {}".format(max_height))
    print("  - mean height: {}".format(average_height))
    print("")
    print("  - number of ranks: {}".format(number_of_ranks))
    print("")

# On affiche les propriétés de chaque processus (rand, coordonnées cartésiennes, voisins)

for ir in range(number_of_ranks):

    if ir == rank:

        print(
            "  - Processus {} : rang = {}, coordonnées = {}, voisins = {} et {}, local N : {}, début : {}, fin (incluse) : {}, point fantôme gauche: {}, point fantôme droit: {}".format(
                rank,
                cart_rank,
                cart_coords,
                left_rank,
                right_rank,
                local_size,
                local_start,
                local_end,
                left_ghost_cell,
                right_ghost_cell,
            )
        )

    comm.Barrier()

# Attend 1 secondes
time.sleep(1)

# _____________________________________
# Matplotlib init

# if (matplotlib_period > 0):

#   fig = plt.figure(figsize=(12, 8))

#   gs = plt.GridSpec(2, 2)
#   ax0 = plt.subplot(gs[0,:])
#   ax1 = plt.subplot(gs[1,:])

# _____________________________________
#
# Time loop
# _____________________________________

# get the time at the beginning of the main loop using MPI
start = MPI.Wtime()

# Local timers
local_timer_p2p_comm = 0
local_timer_reduce_comm = 0
local_timer_diags = 0

if comm.rank == 0:

    print(" --------------------------------------------- ")
    print(" MAIN LOOP")
    print(" -------------------------------------------- ")
    print(" Iteration | max h    | mean h   | water    |")
    print(" ----------| ---------|----------|----------|")

for it in range(iterations):

    #   # Compute height and q at midpoint in time and space

    height_m[0 : local_size - 1] = (
        0.5 * (height[0 : local_size - 1] + height[1:local_size])
        - (0.5 * dt) * (q[1:local_size] - q[0 : local_size - 1]) * invdx
    )

    q_m[0 : local_size - 1] = (
        0.5 * (q[0 : local_size - 1] + q[1:local_size])
        - 0.5
        * dt
        * (
            q[1:local_size] ** 2 / height[1:local_size]
            + 0.5 * g * height[1:local_size] ** 2
            - q[0 : local_size - 1] ** 2 / height[0 : local_size - 1]
            - 0.5 * g * height[0 : local_size - 1] ** 2
        )
        * invdx
    )

    # Advance the height and q to the next time step

    height[1 : local_size - 1] = (
        height[1 : local_size - 1]
        - dt * (q_m[1 : local_size - 1] - q_m[0 : local_size - 2]) * invdx
    )

    q[1 : local_size - 1] = (
        q[1 : local_size - 1]
        - dt
        * (
            q_m[1 : local_size - 1] ** 2 / height_m[1 : local_size - 1]
            + 0.5 * g * height_m[1 : local_size - 1] ** 2
            - q_m[0 : local_size - 2] ** 2 / height_m[0 : local_size - 2]
            - 0.5 * g * height_m[0 : local_size - 2] ** 2
        )
        * invdx
    )

    # Adjust the boundary conditions for ghost cells using sendrecv

    start_p2p_comm = MPI.Wtime()

    tag = 0

    # Sendrecv height
    # height[local_size - 1] = cart_comm.sendrecv(
    #     height[1],
    #     dest=left_rank,
    #     sendtag=tag,
    #     source=right_rank,
    #     recvtag=tag,
    #     status=None,
    # )
    # height[0] = cart_comm.sendrecv(
    #     height[local_size - 2],
    #     dest=right_rank,
    #     sendtag=tag,
    #     source=left_rank,
    #     recvtag=tag,
    #     status=None,
    # )

    # Sendrecv q
    # q[local_size - 1] = cart_comm.sendrecv(
    #     q[1], dest=left_rank, sendtag=tag, source=right_rank, recvtag=tag, status=None
    # )
    # q[0] = cart_comm.sendrecv(
    #     q[local_size - 2],
    #     dest=right_rank,
    #     sendtag=tag,
    #     source=left_rank,
    #     recvtag=tag,
    #     status=None,
    # )

    # Send and recv height using non-blocking communication

    if left_rank !=  MPI.PROC_NULL:
        s1 = cart_comm.isend(height[1], dest=left_rank, tag=tag)
        r2 = cart_comm.irecv( source=left_rank, tag=tag)
    else:
        s1 = MPI.REQUEST_NULL
        r2 = MPI.REQUEST_NULL

    if right_rank !=  MPI.PROC_NULL:
        r1 = cart_comm.irecv( source=right_rank, tag=tag)
        s2 = cart_comm.isend(height[local_size - 2], dest=right_rank, tag=tag)
    else:
        r1 = MPI.REQUEST_NULL
        s2 = MPI.REQUEST_NULL


    # Send and recv q using non-blocking communication

    if left_rank !=  MPI.PROC_NULL:
      s3 = cart_comm.isend(q[1], dest=left_rank, tag=tag)
      r4 = cart_comm.irecv(source=left_rank, tag=tag)
    else:
      s3 = MPI.REQUEST_NULL
      r4 = MPI.REQUEST_NULL

    if right_rank !=  MPI.PROC_NULL:
      s4 = cart_comm.isend(q[local_size - 2], dest=right_rank, tag=tag)
      r3 = cart_comm.irecv(source=right_rank, tag=tag)
    else:
      s4 = MPI.REQUEST_NULL
      r3 = MPI.REQUEST_NULL

    MPI.Request.Waitall([s1, s2, s3, s4])

    height[local_size - 1] = r1.wait()
    height[0] = r2.wait()

    q[local_size - 1] = r3.wait()
    q[0] = r4.wait()

    # MPI barrier
    comm.Barrier()

    local_timer_p2p_comm += MPI.Wtime() - start_p2p_comm

    #  Reflective boundary conditions
    if left_rank == MPI.PROC_NULL:
        height[0] = height[1]
        q[0] = -q[1]

    if right_rank == MPI.PROC_NULL:
        height[local_size - 1] = height[local_size - 2]
        q[local_size - 1] = -q[local_size - 2]

        #   # Periodic boundary conditions
        #   # height[0] = height[N-2]
        #   # height[N-1] = height[1]
        #   # q[0] = q[N-2]
        #   # q[N-1] = q[1]

    # Maximal height
    local_max_height = np.max(height)

    local_timer_reduce_comm_start = MPI.Wtime()
    max_height = comm.reduce(local_max_height, op=MPI.MAX, root=0)
    local_timer_reduce_comm += MPI.Wtime() - local_timer_reduce_comm_start

    # Sum height
    local_sum_height = (
        np.sum(
            height[left_ghost_cell : local_size - 1]
            + height[left_ghost_cell + 1 : local_size]
        )
        * 0.5
    )

    local_timer_reduce_comm_start = MPI.Wtime()
    sum_height = comm.reduce(local_sum_height, op=MPI.SUM, root=0)
    local_timer_reduce_comm += MPI.Wtime() - local_timer_reduce_comm_start

    if rank == 0:
        # Average height
        average_height = sum_height / (N - 1)
        # Water quantity
        water_quantity = sum_height * dx

    # Terminal information
    if (it % print_period == 0 and rank == 0):

        print(
            " {:9d} |   {:2.3f}  |   {:2.3f}  |   {:2.3f}  |".format(
                it, max_height, average_height, water_quantity
            )
        )

    # Output
    # Ce morceau de code permet de sauvegarder l'état de la simulation dans des fichiers binaires

    if output_period > 0 and it % output_period == 0:

        local_timer_diags_start = MPI.Wtime()

        # The global grid is reconstructed from the local grids
        global_height = np.zeros(N)
        global_q = np.zeros(N)

        # We use the MPI gather function to gather the local grids on the global grid

        counts = comm.gather(len(height[left_ghost_cell:local_size - right_ghost_cell]), root=0)
        comm.Gatherv(height[left_ghost_cell:local_size - right_ghost_cell], recvbuf=(global_height, counts), root=0)
            
        counts = np.array(comm.gather(len(q[left_ghost_cell:local_size - right_ghost_cell]), root=0))
        comm.Gatherv(q[left_ghost_cell:local_size - right_ghost_cell], recvbuf=(global_q, counts), root=0)

        # comm.Gather(
        #     height[left_ghost_cell : local_size - right_ghost_cell],
        #     global_height,
        #     root=0,
        # )
        # comm.Gather(
        #     q[left_ghost_cell : local_size - right_ghost_cell], global_q, root=0
        # )

        if rank == 0:

            f = open("diags/diag_{:05d}.bin".format(it), "wb")
            f.write(struct.pack("i", it))
            f.write(struct.pack("d", L))
            f.write(struct.pack("i", N))
            data_h = struct.pack("d", global_height[0])
            data_q = struct.pack("d", global_q[0])
            for h in global_height[1:N]:
                data_h += struct.pack("d", h)
            for q_value in global_q[1:N]:
                data_q += struct.pack("d", q_value)
            f.write(data_h)
            f.write(data_q)
            f.close()

        # MPI barrier
        comm.Barrier()

        local_timer_diags += MPI.Wtime() - local_timer_diags_start

#   # Matplotlib
#   # Ce morceau de code permet d'afficher l'état de la simulation en temps réel

#   if (matplotlib_period > 0 and it%matplotlib_period == 0):

#     ax0.cla()
#     #ax0.plot(x,height)

#     ax0.stackplot(x, height)

#     ax0.set_xlabel("x")
#     ax0.set_ylabel("water height")
#     ax0.set_xlim([0, L])
#     ax0.set_ylim([0,3])

#     ax1.cla()
#     ax1.stackplot(x, q)
#     ax1.set_xlabel("x")
#     ax1.set_ylabel("mass velocity")

#     fig.tight_layout()

#     plt.pause(matplotlib_pause_duration)
#     plt.draw()

end = MPI.Wtime()

# ____________________________________________
#
# Timers
# ____________________________________________

local_timer_main_loop = end - start

# Compute min, mean, max timers using MPI reduce

min_timer_main_loop = comm.reduce(local_timer_main_loop, op=MPI.MIN, root=0)
mean_timer_main_loop = comm.reduce(local_timer_main_loop, op=MPI.SUM, root=0)
max_timer_main_loop = comm.reduce(local_timer_main_loop, op=MPI.MAX, root=0)

min_timer_p2p_com = comm.reduce(local_timer_p2p_comm, op=MPI.MIN, root=0)
mean_timer_p2p_comm = comm.reduce(local_timer_p2p_comm, op=MPI.SUM, root=0)
max_timer_p2p_comm = comm.reduce(local_timer_p2p_comm, op=MPI.MAX, root=0)

min_timer_reduce_comm = comm.reduce(local_timer_reduce_comm, op=MPI.MIN, root=0)
mean_timer_reduce_comm = comm.reduce(local_timer_reduce_comm, op=MPI.SUM, root=0)
max_timer_reduce_comm = comm.reduce(local_timer_reduce_comm, op=MPI.MAX, root=0)

min_timer_diags = comm.reduce(local_timer_diags, op=MPI.MIN, root=0)
mean_timer_diags = comm.reduce(local_timer_diags, op=MPI.SUM, root=0)
max_timer_diags = comm.reduce(local_timer_diags, op=MPI.MAX, root=0)

if comm.rank == 0:

    mean_timer_main_loop /= number_of_ranks
    mean_timer_p2p_comm /= number_of_ranks
    mean_timer_reduce_comm /= number_of_ranks
    mean_timer_diags /= number_of_ranks

    print("")
    print(" ------------------------------------------------ ")
    print(" TIMERS")
    print(" ------------------------------------------------------------------------|")
    print("            code part |  min  (s)  | mean (s)  | max (s)    | percentage |")
    print(" ---------------------|------------|-----------|------------|------------|")

    percentage = mean_timer_main_loop / (mean_timer_main_loop) * 100
    percentage_p2p_comm = mean_timer_p2p_comm / (mean_timer_main_loop) * 100
    percentage_reduce_comm = mean_timer_reduce_comm / (mean_timer_main_loop) * 100
    percentage_diags = mean_timer_diags / (mean_timer_main_loop) * 100

    print(
        " {:>20} | {:>10.3f} | {:>10.3f} | {:>10.3f} | {:>9.1f}% |".format(
            "Main loop",
            min_timer_main_loop,
            mean_timer_main_loop,
            max_timer_main_loop,
            percentage,
        )
    )

    print(
        " {:>20} | {:>10.3f} | {:>10.3f} | {:>10.3f} | {:>9.1f}% |".format(
            "P2P comm",
            min_timer_p2p_com,
            mean_timer_p2p_comm,
            max_timer_p2p_comm,
            percentage_p2p_comm,
        )
    )

    print(
        " {:>20} | {:>10.3f} | {:>10.3f} | {:>10.3f} | {:>9.1f}% |".format(
            "Reduce comm",
            min_timer_reduce_comm,
            mean_timer_reduce_comm,
            max_timer_reduce_comm,
            percentage_reduce_comm,
        )
    )

    print(
        " {:>20} | {:>10.3f} | {:>10.3f} | {:>10.3f} | {:>9.1f}% |".format(
            "Diags",
            min_timer_diags,
            mean_timer_diags,
            max_timer_diags,
            percentage_diags,
        )
    )


# if (matplotlib_period > 0):
#   plt.show()

# On finalise MPI
MPI.Finalize()
