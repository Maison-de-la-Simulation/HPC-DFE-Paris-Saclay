# ______________________________________________________________________________
#
# Read and generate an animation of the grid for all iterations in 3D
#
# ______________________________________________________________________________

import sys
import glob
import struct
import numpy as np
from matplotlib import *
from matplotlib.pyplot import *
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D

# ______________________________________________________________________________
# RCParams - personalize the figure output

rcParams['figure.facecolor'] = 'w'
rcParams['font.size'] = 15
rcParams['xtick.labelsize'] = 15
rcParams['ytick.labelsize'] = 15
rcParams['axes.labelsize'] = 15

rcParams['xtick.major.size'] = 10
rcParams['ytick.major.size'] = 10

rcParams['xtick.minor.size'] = 5
rcParams['ytick.minor.size'] = 5

rcParams['axes.linewidth'] = 1.5

rcParams['xtick.major.width'] = 2
rcParams['ytick.major.width'] = 2

rcParams['xtick.minor.width'] = 1.5
rcParams['ytick.minor.width'] = 1.5

# ______________________________________________________________________________
# Read command line arguments

file_path = ""

if len(sys.argv) > 1:
    file_path = sys.argv[1]
else:
    sys.exit("Please, specify a valid path to a charge hdf5 file as a first command line argument.")

# ______________________________________________________________________________
# Get the list of files

file_list = glob.glob("{}/grid_*.dat".format(file_path))

file_list = sorted(file_list)

print file_list

# ______________________________________________________________________________
# Animation function

def animate(i):

    ax.cla()

    file = open(file_list[i], 'r')

    content = file.read()

    Nx = struct.unpack('i',content[0:4])[0]
    Ny = struct.unpack('i',content[4:8])[0]
    Lx = struct.unpack('d',content[8:16])[0]
    Ly = struct.unpack('d',content[16:24])[0]

    grid = np.zeros([Ny,Nx])

    for iy in range(Ny):
        grid[iy,:] = np.array(struct.unpack('{}d'.format(Nx),content[24 + 8*iy*Nx:24 + 8*(iy+1)*Nx]))

    # inverse of Fortran
    y = np.linspace(0,Ly,Ny)
    x = np.linspace(0,Lx,Nx)

    X, Y = np.meshgrid(x, y)

    # Plot the surface.
    surf = ax.plot_surface(X, Y, grid, cmap=cm.coolwarm,
                           linewidth=0, antialiased=True)

    ax.set_zlim(0, 0.002)

    ax.set_xlabel("Vertice index in x")
    ax.set_ylabel("Vertice index in y")

    t = ax.set_title("Grid at {}".format(file_list[i]))
    t.set_y(1.05)

    return surf

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 8))
ax = fig.gca(projection='3d')

animate(0)

nt = len(file_list)

ani = animation.FuncAnimation(fig, animate, np.arange(0, nt),
                              interval=500, blit=False)

fig.tight_layout()

show()
