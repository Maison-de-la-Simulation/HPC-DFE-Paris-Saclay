# ____________________________________________________________________
#
# SHALLOW WATER 1D
# This program simulates the conservative 1d shallow water equations
#
# Description of the 1d structured discretized domain:
#
#  size is the number of points
#  |
#  |    `size - 1` is therefore the number of cells
#  |    |
#  v    v
#
#  |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|

#  <--> dx is the cell length
#
#  < -------- `length`is the domain length ----------------------->
# ____________________________________________________________________

# Libraries
# _____________________________________

import numpy as np
import matplotlib.pyplot as plt

# Input parameters
# _____________________________________

# space
length = 5.
size = 200

# time
duration = 5.

# gravity
g = 9.8

# Terminal output period
print_period = 10

# Disk output period
output_period = 10

# Matplotlib display period
# if 0, the matplotlib figure display is disabled
matplotlib_period = 10
# Display time for each figure (carreful, it stops the computation)             
matplotlib_pause_duration = 0.1

# Initilisation
# _____________________________________

dx = length / (size-1)
invdx = 1 / dx
x = np.linspace ( 0, length, size )

dt = 0.2 * dx
iterations = int(duration / dt)

# initial heigh of the water: the dam break
height  = np.zeros(size)
for ix, x_value in enumerate(x):
  if (x_value < 0.4 * length):
    height[ix] = 2.
  elif (x_value > 0.6 * length):
    height[ix] = 1.
  else:
    height[ix] = 1 + 0.5*(1. + np.cos(  np.pi * (x_value - 0.4 * length) / (0.2*length)))

# initial heigh of the water: gaussian
# height  = np.zeros(size)
# for ix, x_value in enumerate(x):
#   if (x_value < 0.4 * length):
#     height[ix] = 1.
#   elif (x_value > 0.6 * length):
#     height[ix] = 1.
#   else:
#     height[ix] = 1. + np.exp( - (x_value - 0.5*length)**2 / (0.1 * length)**2)

# mass velocity = ordinary velocity times water heigth
uh = np.zeros(size)

# height at mid-point and half time steps
hm = np.zeros ( size-1 )

# mass velocity at mid-point and half time steps
uhm = np.zeros (size-1 )

# Adjust the boundary conditions for init
height[0] = height[1]
height[size-1] = height[size-2]
# height[0] = height[size-2]
# height[size-1] = height[1]

# Matplotlib init
# _____________________________________

if (matplotlib_period > 0):

  fig = plt.figure(figsize=(12, 6))

  gs = plt.GridSpec(2, 2)
  ax0 = plt.subplot(gs[0,:])
  ax1 = plt.subplot(gs[1,:])

# Time loop
# _____________________________________

for it in range (iterations):
#
#  Take a half time step, estimating H and UH at the size-1 spatial midpoints.
#
    hm[0:size-1] = 0.5 * ( height[0:size-1] + height[1:size] ) - ( 0.5 * dt ) * ( uh[1:size] - uh[0:size-1] ) * invdx

    uhm[0:size-1] = 0.5 * ( uh[0:size-1] + uh[1:size] )  \
      - 0.5 * dt * ( uh[1:size] ** 2    / height[1:size]   + 0.5 * g * height[1:size] ** 2 \
      - uh[0:size-1] ** 2  / height[0:size-1] - 0.5 * g * height[0:size-1] ** 2 ) * invdx
#
#  Take a full time step, evaluating the derivative at the half time step,
#  to estimate the solution at the size-2 nodes.
#
    height[1:size-1] = height[1:size-1] \
      - dt * ( uhm[1:size-1] - uhm[0:size-2] ) / dx

    uh[1:size-1] = uh[1:size-1] \
      - dt * ( \
        uhm[1:size-1] ** 2  / hm[1:size-1] + 0.5 * g * hm[1:size-1] ** 2 \
      - uhm[0:size-2] ** 2  / hm[0:size-2] - 0.5 * g * hm[0:size-2] ** 2 ) / dx

    #  Reflective boundary conditions
    height[0] = height[1]
    height[size-1] = height[size-2]
    uh[0] = - uh[1]
    uh[size-1] = - uh[size-2]

    # Periodic boundary conditions
    # height[0] = height[size-2]
    # height[size-1] = height[1]
    # uh[0] = uh[size-2]
    # uh[size-1] = uh[1]

    # Maximal height
    max_height = np.max(height)
    average_height = np.average(height)
    water_quantity = np.sum(height[1:size] + height[0:size-1]) * dx * 0.5

    # Terminal information
    if (it%print_period == 0):
        print(" - iteration {:5d} - max height: {:2.3f} - mean height: {:2.3f} - water quantity: {:2.3f}".format(it, max_height, average_height, water_quantity))

    # Output 

    # Matplotlib

    if (matplotlib_period > 0 and it%matplotlib_period == 0):

      ax0.cla()
      #ax0.plot(x,height)

      ax0.stackplot(x, height)

      ax0.set_xlabel("x")
      ax0.set_ylabel("water height")
      ax0.set_xlim([0, length])
      ax0.set_ylim([0,3])

      ax1.cla()
      ax1.stackplot(x, uh)
      ax1.set_xlabel("x")
      ax1.set_ylabel("mass velocity")

      fig.tight_layout()

      plt.pause(matplotlib_pause_duration)
      plt.draw()

    # plt.show()

