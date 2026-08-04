# ____________________________________________________________________
#
# MANDELBULB

# This program simulates the Mandelbulb fractal 
# (3D version of the Mandelbrot problem)
#
# ____________________________________________________________________

# ____________________________________________________________________
#
# Libraries
# ____________________________________________________________________

import numpy as np
import math as m
import vtk
import argparse
import time

# ____________________________________________________________________
#
# MPI
# ____________________________________________________________________

import mpi4py

# En Python, l'initialisation de MPI est automatique mais ce n'est pas le cas des autres langages comme C, C++ ou Fortran, on désactive donc l'initialisation automatique pour l'exemple :
mpi4py.rc.initialize = False
mpi4py.rc.finalize = False

# En Python, la bibliothèque MPI se récupère dans le paquet mpi4py
from mpi4py import MPI

# ____________________________________________________________________
#
# Input parameters
# ____________________________________________________________________

# Domain size in each direction
domain_size = [128, 128, 128]

# domain min
domain_min = [-1.1, -1.1, -1.1]

# domain max
domain_max = [1.1, 1.1, 1.1]

# Number of iterations
iterations = 50

# Initial orbit
initial_position = [0.0, 0.0, 0.0]

# Bailout value
bailout = 2.0

# Power
n = 8

# 
ranks_per_directions = [2, 2, 1]

# ____________________________________________________________________
#
# Command line arguments
# ____________________________________________________________________

parser = argparse.ArgumentParser(description='Mandelbulb')
parser.add_argument('--domain_size', type=int, nargs=3, help='Domain size in each direction')
parser.add_argument('--domain_min', type=float, nargs=3, help='Domain min')
parser.add_argument('--domain_max', type=float, nargs=3, help='Domain max')
parser.add_argument('--iterations', type=int, help='Number of iterations')
parser.add_argument('--initial_position', type=float, nargs=3, help='Initial orbit')
parser.add_argument('--bailout', type=float, help='Bailout value')
parser.add_argument('--power', type=int, help='Power')
parser.add_argument('--ranks_per_dir', type=int, nargs=3, help='Number of ranks per direction')

args = parser.parse_args()

if args.domain_size:
    domain_size = args.domain_size

if args.domain_min:
    domain_min = args.domain_min

if args.domain_max:
    domain_max = args.domain_max

if args.iterations:
    iterations = args.iterations

if args.initial_position:
    initial_position = args.initial_position

if args.bailout:
    bailout = args.bailout

if args.power:
    n = args.power

if args.ranks_per_dir:
    ranks_per_directions = args.ranks_per_dir

# step size
delta = [ (domain_max[0] - domain_min[0]) / domain_size[0],
          (domain_max[1] - domain_min[1]) / domain_size[1],
          (domain_max[2] - domain_min[2]) / domain_size[2] ]


# ____________________________________________________________________
#
# MPI initialization
# ____________________________________________________________________

MPI.Init()

# On récupère le communicateur global
comm = MPI.COMM_WORLD

# On récupère le nombre total de rang dans le communicateur par défaut
number_of_ranks = comm.Get_size()

# On récupère le rang du processus courant
rank = comm.Get_rank()

# Création d'un communicateur cartésien 1D
ndims = 3

# On définit si les dimensions sont périodiques ou non
periods = [False,False,False]

# On crée le communicateur cartésien
cart_comm = comm.Create_cart(ranks_per_directions, periods, reorder=True)

# On récupère le rang dans le communicateur cartésien
cart_rank = cart_comm.Get_rank()

# On récupère les coordonnées du rang dans le communicateur cartésien
cart_coords = cart_comm.Get_coords(cart_rank)

# On récupère le rang du processus voisin dans la dimension 0
# avec un décalage de +1
neighbors = [ cart_comm.Shift(0, 1)[0], cart_comm.Shift(0, 1)[1], cart_comm.Shift(1, 1)[0], cart_comm.Shift(1, 1)[1], cart_comm.Shift(2, 1)[0], cart_comm.Shift(2, 1)[1] ]

# ____________________________________________________________________
#
# Initialization
# ____________________________________________________________________

# Local domain size
local_domain_size = [domain_size[0]//ranks_per_directions[0], 
                     domain_size[1]//ranks_per_directions[1],
                     domain_size[2]//ranks_per_directions[2]]

# first index
local_first_index = [cart_coords[0] * local_domain_size[0],
                     cart_coords[1] * local_domain_size[1],
                     cart_coords[2] * local_domain_size[2]]

# last index
local_last_index = [ local_first_index[0] + local_domain_size[0],
                     local_first_index[1] + local_domain_size[1],
                     local_first_index[2] + local_domain_size[2]]

# step size
delta = [ (domain_max[0] - domain_min[0]) / domain_size[0],
          (domain_max[1] - domain_min[1]) / domain_size[1],
          (domain_max[2] - domain_min[2]) / domain_size[2] ]

# Initialize the domain array
domain_array = np.zeros(local_domain_size)

# Initialize the iteration array
iteration_array = np.zeros(local_domain_size, dtype=int)

# Initialize the distance estimator array
distance_array = np.zeros(local_domain_size)

# Total indexes
total_indexes = domain_size[0]*domain_size[1]*domain_size[2]

# ____________________________________________________________________
#
# Terminal output summary
# ____________________________________________________________________

if cart_comm.rank == 0:

    print(" ____________________________________________________________ ")
    print(" ")
    print(" Mandelbulb")
    print(" ____________________________________________________________ ")
    print(" ")
    print(" > Parameters summary:")
    print(" - Domain size: {}".format(domain_size))
    print(" - Domain min: {}".format(domain_min))
    print(" - Domain max: {}".format(domain_max))
    print(" - Delta: {}".format(delta))
    print(" - Number of iterations: {}".format(iterations))
    print(" - Initial position: {}".format(initial_position))
    print(" - Bailout: {}".format(bailout))
    print(" - Power: {}".format(n))

    print(" > MPI properties:")
    print(" - Number of ranks: {}".format(number_of_ranks))
    print(" - Number of ranks per direction: {}".format(ranks_per_directions))

# We show the properties of each rank

for ir in range(number_of_ranks):

    if ir == rank:

        print(
            "  - Processus {} : rank = {}, coordinates = {}, neighbors = {}, local N : {}, first index : {}, last index : {}".format(
                rank,
                cart_rank,
                cart_coords,
                neighbors,
                local_domain_size[0],
                local_first_index,
                local_last_index,
            )
        )

    cart_comm.Barrier()

if cart_comm.rank == 0:

    memory = total_indexes* (8 + 8 + 4) /(1024.*1024)

    print(" ")
    print(" > Total memory: {} MB".format(memory))

# ____________________________________________________________________
#
# Mandelbulb computation
# ____________________________________________________________________

# For percentage display
percentage_index = 0
local_total_index = local_domain_size[0]*domain_size[1]*domain_size[2]

if cart_comm.rank == 0:
    print(" ")
    print(" > Processing the domain:")

# get the time at the beginning of the main loop
start = MPI.Wtime()

# Loop over the domain pixels
for local_ix in range(local_domain_size[0]):
    for local_iy in range(local_domain_size[1]):
        for local_iz in range(local_domain_size[2]):

            # translate the local index into global index
            ix = local_ix + local_first_index[0]
            iy = local_iy + local_first_index[1]
            iz = local_iz + local_first_index[2]

            # Pixel position
            pixel_position = [domain_min[0] + ix * delta[0] + delta[0]*0.5,
                              domain_min[1] + iy * delta[1] + delta[1]*0.5,
                              domain_min[2] + iz * delta[2] + delta[2]*0.5]
            
            r = 0.0
            dr = 1.0

            # Compute the pixel value
            position = initial_position

            for i in range(iterations):
                
                r2 = position[0]**2 + position[1]**2 + position[2]**2

                # Check the bailout condition
                if r2 > bailout:
                    domain_array[local_ix, local_iy, local_iz] = 1.0
                    iteration_array[local_ix, local_iy, local_iz] = i
                    r = m.sqrt(r2)
                    dr =  r**(n-1.0)*n*dr + 1.0
                    distance_array[local_ix, local_iy, local_iz] = 0.5*m.log(r)*r/dr
                    break

                r = m.sqrt(r2)

                dr =  r**(n-1.0)*n*dr + 1.0

                theta = m.atan2(np.sqrt(position[0]**2 + position[1]**2), position[2])

                phi = m.atan2(position[1], position[0])

                position = [
                        r**n * m.sin(theta*n) * m.cos(phi*n) + pixel_position[0],
                        r**n * m.sin(theta*n) * m.sin(phi*n) + pixel_position[1],
                        r**n * m.cos(theta*n)                + pixel_position[2]
                        ]

                # print(" it = {}, pixel {}, position = {}, r={}, dr = {}".format(i, pixel_position, position, r, dr))
            
            #domain_array[ix, iy, iz] = 0.5*m.log(r)*r/dr
            

        global_index = local_iz + local_iy*domain_size[2] + local_ix*domain_size[1]*domain_size[2]

        if (global_index > (percentage_index * local_total_index * 0.1)):

            if cart_comm.rank == 0:
                print("  - rank {} at {}% ({} {} {})".format(rank, percentage_index*10, ix, iy, iz))
                percentage_index += 1

        #print("Processing pixel ({},{},{})".format(ix, iy, iz))

# Wait for all processes
cart_comm.Barrier()

# get the time at the end of the main loop
end = MPI.Wtime()

# ____________________________________________________________________
#
# Compute the volume of the Mandelbulb
# ____________________________________________________________________

time_start_volume = MPI.Wtime()

local_volume = np.sum(domain_array)*delta[0]*delta[1]*delta[2]

# Reduce the local volume to the global volume
volume = cart_comm.reduce(local_volume, op=MPI.SUM, root=0)

time_end_volume = MPI.Wtime()

if cart_comm.rank == 0:
    print(" ")
    print(" > Mandelbulb volume: {}".format(volume))

# ____________________________________________________________________
#
# Print the time
# ____________________________________________________________________

mean_time_loop = cart_comm.reduce(end - start, op=MPI.SUM, root=0)
mean_time_loop = mean_time_loop/number_of_ranks
min_time_loop = cart_comm.reduce(end - start, op=MPI.MIN, root=0)
max_time_loop = cart_comm.reduce(end - start, op=MPI.MAX, root=0)

mean_time_volume = cart_comm.reduce(time_end_volume - time_start_volume, op=MPI.SUM, root=0)
mean_time_volume = mean_time_volume/number_of_ranks
min_time_volume = cart_comm.reduce(time_end_volume - time_start_volume, op=MPI.MIN, root=0)
max_time_volume = cart_comm.reduce(time_end_volume - time_start_volume, op=MPI.MAX, root=0)

if cart_comm.rank == 0:
    print(" > Time to process the Mandelbulb: mean = {}, min = {}, max = {}".format(mean_time_loop, min_time_loop, max_time_loop))
    print(" > Time to compute the volume: mean = {}, min = {}, max = {}".format(mean_time_volume, min_time_volume, max_time_volume))

# ____________________________________________________________________
#
# Save the domain array using the vtk format
# ____________________________________________________________________

# Gather the local domains in a rank 0

if cart_comm.rank == 0:

    local_domains_in_rank_0 = np.zeros([number_of_ranks, local_domain_size[0], local_domain_size[1], local_domain_size[2]])
    local_iterations_in_rank_0 = np.zeros([number_of_ranks, local_domain_size[0], local_domain_size[1], local_domain_size[2]], dtype=int)
    local_distance_in_rank_0 = np.zeros([number_of_ranks, local_domain_size[0], local_domain_size[1], local_domain_size[2]])

else:

    local_domains_in_rank_0 = None
    local_iterations_in_rank_0 = None
    local_distance_in_rank_0 = None

# sendcounts = local_domain_size[0]*local_domain_size[1]*local_domain_size[2]

# displacements = rank*local_domain_size[0]*local_domain_size[1]*local_domain_size[2]

sendcounts = [local_domain_size[0]*local_domain_size[1]*local_domain_size[2] for i in range(number_of_ranks)]

displacements = [i*local_domain_size[0]*local_domain_size[1]*local_domain_size[2] for i in range(number_of_ranks)]

cart_comm.Gatherv(domain_array, [local_domains_in_rank_0, sendcounts, displacements, MPI.DOUBLE], root=0)
cart_comm.Gatherv(iteration_array, [local_iterations_in_rank_0, sendcounts, displacements, MPI.DOUBLE], root=0)
cart_comm.Gatherv(distance_array, [local_distance_in_rank_0, sendcounts, displacements, MPI.DOUBLE], root=0)

# Reconstruction of the global domain array
# Each rank has a local domain array representing a local matrix block of size local_domain_size

if cart_comm.rank == 0:

    global_domain = np.zeros(domain_size)
    global_iteration = np.zeros(domain_size, dtype=int)
    global_distance = np.zeros(domain_size)

    for irank in range(number_of_ranks):

        #print("rank {}".format(irank))

        # get the coordinates of the rank in the cartesian communicator
        coords = cart_comm.Get_coords(irank)

        # Index in the global domain
        ix_start = coords[0]*local_domain_size[0]
        iy_start = coords[1]*local_domain_size[1]
        iz_start = coords[2]*local_domain_size[2]

        ix_end = ix_start + local_domain_size[0]
        iy_end = iy_start + local_domain_size[1]
        iz_end = iz_start + local_domain_size[2]

        # Put the local domain in the global domain taking 
        # into account the local indexes and the local domain size

        for ix in range(local_domain_size[0]):
            for iy in range(local_domain_size[1]):
                for iz in range(local_domain_size[2]):

                    global_domain[ix_start+ix, iy_start+iy, iz_start+iz] = local_domains_in_rank_0[irank, ix, iy, iz]
                    global_iteration[ix_start+ix, iy_start+iy, iz_start+iz] = local_iterations_in_rank_0[irank, ix, iy, iz]
                    global_distance[ix_start+ix, iy_start+iy, iz_start+iz] = local_distance_in_rank_0[irank, ix, iy, iz]

        # global_domain[ix_start:ix_end, iy_start:iy_end, iz_start:iz_end] = local_domains_in_rank_0[rank, :, :, :]

if cart_comm.rank == 0:

    # Create a vtkImageData object
    imageData = vtk.vtkImageData()
    imageData.SetDimensions(domain_size[0], domain_size[1], domain_size[2])
    imageData.SetOrigin(domain_min[0], domain_min[1], domain_min[2])
    imageData.SetSpacing(delta[0], delta[1], delta[2])

    # Create a vtkFloatArray object to store the domain array
    scalars = vtk.vtkFloatArray()
    scalars.SetNumberOfComponents(1)
    scalars.SetName("Mandelbulb")
    for iz in range(domain_size[2]):
        for iy in range(domain_size[1]):
            for ix in range(domain_size[0]):
                scalars.InsertNextValue(global_domain[ix, iy, iz])
    imageData.GetPointData().SetScalars(scalars)

    # Create a vtkFloatArray object to store the iteration array
    scalars = vtk.vtkIntArray()
    scalars.SetNumberOfComponents(1)
    scalars.SetName("Iterations")
    for iz in range(domain_size[2]):
        for iy in range(domain_size[1]):
            for ix in range(domain_size[0]):
                scalars.InsertNextValue(global_iteration[ix, iy, iz])
    imageData.GetPointData().AddArray(scalars)

    # Create a vtkFloatArray object to store the distance array
    scalars = vtk.vtkFloatArray()
    scalars.SetNumberOfComponents(1)
    scalars.SetName("Distance")
    for iz in range(domain_size[2]):
        for iy in range(domain_size[1]):
            for ix in range(domain_size[0]):
                scalars.InsertNextValue(global_distance[ix, iy, iz])
    imageData.GetPointData().AddArray(scalars)

    # Write the vtkImageData object to a file
    writer = vtk.vtkStructuredPointsWriter()
    writer.SetFileName("mandelbulb.vtk")
    writer.SetInputData(imageData)
    writer.Write()

# ____________________________________________________________________
#
# finalize MPI
# ____________________________________________________________________

MPI.Finalize()
            

