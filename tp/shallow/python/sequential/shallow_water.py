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

# ____________________________________________________________________
#
# Libraries
# ____________________________________________________________________

import numpy as np
import matplotlib.pyplot as plt
import struct
import os
import time

# ____________________________________________________________________
#
# Input parameters
# ____________________________________________________________________

# Space (L)
length = 5.

# Number of points (N)
size = 10000

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

# ____________________________________________________________________
#
# Initialization
# ____________________________________________________________________

dx = length / (size-1)
invdx = 1 / dx
x = np.linspace ( 0, length, size )

dt = 0.1 * dx
duration = iterations * dt

# initial heigh of the water: the dam break
height  = np.zeros(size)
for ix, x_value in enumerate(x):
  if (x_value < 0.4 * length):
    height[ix] = 2.
  elif (x_value > 0.6 * length):
    height[ix] = 1.
  else:
    height[ix] = 1 + 0.5*(1. + np.cos(  np.pi * (x_value - 0.4 * length) / (0.2*length)))

# initial heigh of the water: Gaussian
# height  = np.zeros(size)
# for ix, x_value in enumerate(x):
#   if (x_value < 0.4 * length):
#     height[ix] = 1.
#   elif (x_value > 0.6 * length):
#     height[ix] = 1.
#   else:
#     height[ix] = 1. + np.exp( - (x_value - 0.5*length)**2 / (0.1 * length)**2)

# water flow = ordinary velocity u(x,t) times water heigth h(x,t)
q = np.zeros(size)

# height at mid-point and half time steps
height_m = np.zeros ( size-1 )

# water flow at mid-point and half time steps
q_m = np.zeros ( size-1 )

# Adjust the boundary conditions for init
height[0] = height[1]
height[size-1] = height[size-2]
# height[0] = height[size-2]
# height[size-1] = height[1]

# Maximal height value
max_height = np.max(height)
# Sum height value
sum_height = np.sum(height[1:size] + height[0:size-1]) * 0.5
# Average height value
average_height = sum_height / (size-1)
# Water quantity value
water_quantity = sum_height * dx

# Output directory management
if (output_period > 0):
  if not os.path.exists("diags"):
    os.mkdir("diags")

# ____________________________________________________________________
#
# Terminal summary
# ____________________________________________________________________

print(" ------------------------------------------------------------------------- ")
print(" SHALLOW WATER 1D" )
print(" ------------------------------------------------------------------------- ")
print("")
print("  - length: {}".format(length))
print("  - size: {}".format(size))
print("  - dx: {}".format(dx))
print("  - duration: {}".format(duration))
print("  - iterations: {}".format(iterations))
print("  - dt: {}".format(dt))
print("  - print period: {}".format(print_period))
print("  - max height: {}".format(max_height) )
print("  - mean height: {}".format(average_height) )
print("")

# _____________________________________
# Matplotlib init

if (matplotlib_period > 0):

  fig = plt.figure(figsize=(12, 8))

  gs = plt.GridSpec(2, 2)
  ax0 = plt.subplot(gs[0,:])
  ax1 = plt.subplot(gs[1,:])

# ____________________________________________________________________
#
# Time loop
# ____________________________________________________________________

# get the time at the beginning of the main loop
start = time.time()

print(" --------------------------------------------- ")
print(" MAIN LOOP")
print(" -------------------------------------------- ")
print(" Iteration | max h    | mean h   | water    |")
print(" ----------| ---------|----------|----------|")

for it in range (iterations):

  # Compute height and q at midpoint in time and space

  height_m[0:size-1] = 0.5 * ( height[0:size-1] + height[1:size] ) - 0.5 * dt * invdx  * ( q[1:size] - q[0:size-1] )

  q_m[0:size-1] = 0.5 * ( q[0:size-1] + q[1:size] )  \
    - 0.5 * dt * invdx * ( q[1:size] ** 2    / height[1:size]   + 0.5 * g * height[1:size] ** 2 \
    - q[0:size-1] ** 2  / height[0:size-1] - 0.5 * g * height[0:size-1] ** 2 ) 

  # Advance the height and q to the next time step

  height[1:size-1] = height[1:size-1] - dt * invdx * ( q_m[1:size-1] - q_m[0:size-2] ) 

  q[1:size-1] = q[1:size-1] \
    - dt * invdx * ( q_m[1:size-1] ** 2  / height_m[1:size-1] + 0.5 * g * height_m[1:size-1] ** 2 \
    - q_m[0:size-2] ** 2  / height_m[0:size-2] - 0.5 * g * height_m[0:size-2] ** 2 ) 

  #  Reflective boundary conditions
  height[0] = height[1]
  height[size-1] = height[size-2]
  q[0] = - q[1]
  q[size-1] = - q[size-2]

  # Periodic boundary conditions
  # height[0] = height[size-2]
  # height[size-1] = height[1]
  # q[0] = q[size-2]
  # q[size-1] = q[1]

  # Terminal information
  if (it%print_period == 0):
      
    # Maximal height
    max_height = np.max(height)
    # Sum height
    sum_height = np.sum(height[1:size] + height[0:size-1]) * 0.5
    # Average height
    average_height = sum_height / (size-1)
    # Water quantity
    water_quantity = sum_height * dx

    print(" {:9d} |   {:2.3f}  |   {:2.3f}  |   {:2.3f}  |".format(it, max_height, average_height, water_quantity))

  # Output 
  # Ce morceau de code permet de sauvegarder l'état de la simulation dans des fichiers binaires

  if (output_period > 0 and it%output_period == 0):

    f = open('diags/diag_{:05d}.bin'.format(it), 'wb')
    f.write(struct.pack('i',it))
    f.write(struct.pack('d',length))
    f.write(struct.pack('i',size))
    data_h = struct.pack('d', height[0])
    data_q = struct.pack('d', q[0])
    for h in height[1:size]:
      data_h += (struct.pack('d', h))
    for q_value in q[1:size]:
      data_q += (struct.pack('d', q_value))
    f.write(data_h)
    f.write(data_q)
    f.close()

  # Matplotlib
  # Ce morceau de code permet d'afficher l'état de la simulation en temps réel

  if (matplotlib_period > 0 and it%matplotlib_period == 0):

    ax0.cla()
    #ax0.plot(x,height)

    ax0.stackplot(x, height)

    ax0.set_xlabel("x")
    ax0.set_ylabel("water height")
    ax0.set_xlim([0, length])
    ax0.set_ylim([0,3])

    ax1.cla()
    ax1.stackplot(x, q)
    ax1.set_xlabel("x")
    ax1.set_ylabel("mass velocity")

    fig.tight_layout()

    plt.pause(matplotlib_pause_duration)
    plt.draw()

end = time.time()

# ____________________________________________
#
# Timers
# ____________________________________________

timer_main_loop = end - start

print("")
print(" ------------------------------------------------ ")
print(" TIMERS")
print(" ------------------------------------------------ ")
print("            code part |  time (s)  | percentage |")
print(" ---------------------|------------|----------- |")

percentage = timer_main_loop / (timer_main_loop) * 100
print(" {:>20} | {:>10.3f} | {:>9.1f}% |".format("Main loop", timer_main_loop, percentage))

if (matplotlib_period > 0):
  plt.show()  


