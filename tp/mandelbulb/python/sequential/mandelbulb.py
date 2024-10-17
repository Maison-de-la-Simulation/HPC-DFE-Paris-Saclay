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

# ____________________________________________________________________
#
# Initialization
# ____________________________________________________________________

# step size
delta = [ (domain_max[0] - domain_min[0]) / domain_size[0],
          (domain_max[1] - domain_min[1]) / domain_size[1],
          (domain_max[2] - domain_min[2]) / domain_size[2] ]

# Initialize the domain array
domain_array = np.zeros(domain_size)

# Initialize the iteration array
iteration_array = np.zeros(domain_size, dtype=int)

# Initialize the distance estimator array
distance_array = np.zeros(domain_size)

# Total indexes
total_indexes = domain_size[0]*domain_size[1]*domain_size[2]

# ____________________________________________________________________
#
# Terminal output summary
# ____________________________________________________________________

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

memory = domain_size[0]*domain_size[1]*domain_size[2]* (8 + 8 + 4) /(1024.*1024)

print(" ")
print(" > Total memory: {} MB".format(memory))

# ____________________________________________________________________
#
# Mandelbulb computation
# ____________________________________________________________________

# For percentage display
percentage_index = 0

print(" ")
print(" > Processing the domain:")

# get the time at the beginning of the main loop
start = time.time()

# Loop over the domain pixels
for ix in range(domain_size[0]):
    for iy in range(domain_size[1]):
        for iz in range(domain_size[2]):

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
                    domain_array[ix, iy, iz] = 1.0
                    iteration_array[ix, iy, iz] = i
                    r = m.sqrt(r2)
                    dr =  r**(n-1.0)*n*dr + 1.0
                    distance_array[ix, iy, iz] = 0.5*m.log(r)*r/dr
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
            

        global_index = iz + iy*domain_size[2] + ix*domain_size[1]*domain_size[2]

        if (global_index > (percentage_index * total_indexes * 0.1)):
            print(" - progress: {}% ({} {} {})".format(percentage_index*10, ix, iy, iz))
            percentage_index += 1

        #print("Processing pixel ({},{},{})".format(ix, iy, iz))

# get the time at the end of the main loop
end = time.time()

# ____________________________________________________________________
#
# Compute the volume of the Mandelbulb
# ____________________________________________________________________

volume = np.sum(domain_array)*delta[0]*delta[1]*delta[2]

print(" ")
print(" > Mandelbulb volume: {}".format(volume))

# ____________________________________________________________________
#
# Print the time
# ____________________________________________________________________

print(" > Time to process the Mandelbulb: {} seconds".format(end - start))

# ____________________________________________________________________
#
# Save the domain array using the vtk format
# ____________________________________________________________________

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
            scalars.InsertNextValue(domain_array[ix, iy, iz])
imageData.GetPointData().SetScalars(scalars)

# Create a vtkFloatArray object to store the iteration array
scalars = vtk.vtkFloatArray()
scalars.SetNumberOfComponents(1)
scalars.SetName("Iterations")
for iz in range(domain_size[2]):
    for iy in range(domain_size[1]):
        for ix in range(domain_size[0]):
            scalars.InsertNextValue(iteration_array[ix, iy, iz])
imageData.GetPointData().AddArray(scalars)

# Create a vtkFloatArray object to store the distance array
scalars = vtk.vtkFloatArray()
scalars.SetNumberOfComponents(1)
scalars.SetName("Distance")
for iz in range(domain_size[2]):
    for iy in range(domain_size[1]):
        for ix in range(domain_size[0]):
            scalars.InsertNextValue(distance_array[ix, iy, iz])
imageData.GetPointData().AddArray(scalars)

# Write the vtkImageData object to a file
writer = vtk.vtkStructuredPointsWriter()
writer.SetFileName("mandelbulb.vtk")
writer.SetInputData(imageData)
writer.Write()
            

