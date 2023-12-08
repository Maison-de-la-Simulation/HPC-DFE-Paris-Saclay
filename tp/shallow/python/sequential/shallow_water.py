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
#  < -------- `L`is the domain length ---------------------------->
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
import sys

# ____________________________________________________________________
#
# Input parameters
# ____________________________________________________________________

# Space domain length
L = 5.0

# Number of points
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

# ____________________________________________________________________
#
# Initialization
# ____________________________________________________________________

dx = L / (N - 1)
invdx = 1 / dx
x = np.linspace(0, L, N)

dt = 0.1 * dx
duration = iterations * dt

# initial heigh of the water: the dam break
height = np.zeros(N)
for ix, x_value in enumerate(x):
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
q = np.zeros(N)

# height at mid-point and half time steps
height_m = np.zeros(N - 1)

# water flow at mid-point and half time steps
q_m = np.zeros(N - 1)

# Adjust the boundary conditions for init
height[0] = height[1]
height[N - 1] = height[N - 2]
# height[0] = height[N-2]
# height[N-1] = height[1]

# Maximal height value
max_height = np.max(height)
# Sum height value
sum_height = np.sum(height[1:N] + height[0 : N - 1]) * 0.5
# Average height value
average_height = sum_height / (N - 1)
# Water quantity value
water_quantity = sum_height * dx

# Output directory management
if output_period > 0:
    if not os.path.exists("diags"):
        os.mkdir("diags")

# ____________________________________________________________________
#
# Terminal summary
# ____________________________________________________________________

print(" ------------------------------------------------------------------------- ")
print(" SHALLOW WATER 1D")
print(" ------------------------------------------------------------------------- ")
print("")
print("  - domain length (L): {}".format(L))
print("  - number of points (N): {}".format(N))
print("  - dx: {}".format(dx))
print("  - duration: {}".format(duration))
print("  - iterations: {}".format(iterations))
print("  - dt: {}".format(dt))
print("  - print period: {}".format(print_period))
print("  - max height: {}".format(max_height))
print("  - mean height: {}".format(average_height))
print("")

# _____________________________________
# Matplotlib init

if matplotlib_period > 0:

    fig = plt.figure(figsize=(12, 8))

    gs = plt.GridSpec(2, 2)
    ax0 = plt.subplot(gs[0, :])
    ax1 = plt.subplot(gs[1, :])

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

for it in range(iterations):

    # Compute height and q at midpoint in time and space

    height_m[0 : N - 1] = 0.5 * (height[0 : N - 1] + height[1:N]) - 0.5 * dt * invdx * (
        q[1:N] - q[0 : N - 1]
    )

    q_m[0 : N - 1] = 0.5 * (q[0 : N - 1] + q[1:N]) - 0.5 * dt * invdx * (
        q[1:N] ** 2 / height[1:N]
        + 0.5 * g * height[1:N] ** 2
        - q[0 : N - 1] ** 2 / height[0 : N - 1]
        - 0.5 * g * height[0 : N - 1] ** 2
    )

    # Advance the height and q to the next time step

    height[1 : N - 1] = height[1 : N - 1] - dt * invdx * (
        q_m[1 : N - 1] - q_m[0 : N - 2]
    )

    q[1 : N - 1] = q[1 : N - 1] - dt * invdx * (
        q_m[1 : N - 1] ** 2 / height_m[1 : N - 1]
        + 0.5 * g * height_m[1 : N - 1] ** 2
        - q_m[0 : N - 2] ** 2 / height_m[0 : N - 2]
        - 0.5 * g * height_m[0 : N - 2] ** 2
    )

    #  Reflective boundary conditions
    height[0] = height[1]
    height[N - 1] = height[N - 2]
    q[0] = -q[1]
    q[N - 1] = -q[N - 2]

    # Periodic boundary conditions
    # height[0] = height[N-2]
    # height[N-1] = height[1]
    # q[0] = q[N-2]
    # q[N-1] = q[1]

    # Global parameters computation

    # Maximal height
    max_height = np.max(height)
    # Sum height
    sum_height = np.sum(height[1:N] + height[0 : N - 1]) * 0.5
    # Average height
    average_height = sum_height / (N - 1)
    # Water quantity
    water_quantity = sum_height * dx

    # Terminal information
    if it % print_period == 0:

        print(
            " {:9d} |   {:2.3f}  |   {:2.3f}  |   {:2.3f}  |".format(
                it, max_height, average_height, water_quantity
            )
        )

    # Output
    # This piece of code allows to save the state of the simulation in binary files

    if output_period > 0 and it % output_period == 0:

        f = open("diags/diag_{:05d}.bin".format(it), "wb")
        f.write(struct.pack("i", it))
        f.write(struct.pack("d", L))
        f.write(struct.pack("i", N))
        data_h = struct.pack("d", height[0])
        data_q = struct.pack("d", q[0])
        for h in height[1:N]:
            data_h += struct.pack("d", h)
        for q_value in q[1:N]:
            data_q += struct.pack("d", q_value)
        f.write(data_h)
        f.write(data_q)
        f.close()

    # Matplotlib
    # This piece of code allows to display the state of the simulation in real time

    if matplotlib_period > 0 and it % matplotlib_period == 0:

        ax0.cla()
        # ax0.plot(x,height)

        ax0.stackplot(x, height)

        ax0.set_xlabel("x")
        ax0.set_ylabel("water height")
        ax0.set_xlim([0, L])
        ax0.set_ylim([0, 3])

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
print(
    " {:>20} | {:>10.3f} | {:>9.1f}% |".format("Main loop", timer_main_loop, percentage)
)

if matplotlib_period > 0:
    plt.show()
