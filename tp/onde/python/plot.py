# ______________________________________________________________________________
#
# Read and plot the grid at a specific iteration
#
# ______________________________________________________________________________

import sys
import numpy as np
import struct
from matplotlib import *
from matplotlib.pyplot import *

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
    sys.exit("Please, specify a valid path to the file as a first command line argument.")


# ______________________________________________________________________________
# Read the hdf5 file

file = open(file_path, 'r')

content = file.read()

Nx = struct.unpack('i',content[0:4])[0]
Ny = struct.unpack('i',content[4:8])[0]
Lx = struct.unpack('d',content[8:16])[0]
Ly = struct.unpack('d',content[16:24])[0]

print("Nx: {}".format(Nx))
print("Ny: {}".format(Ny))
print("Lx: {}".format(Lx))
print("Ly: {}".format(Ly))

grid = np.zeros([Ny,Nx])

for iy in range(Ny):
    grid[iy,:] = np.array(struct.unpack('{}d'.format(Nx),content[24 + 8*iy*Nx:24 + 8*(iy+1)*Nx]))

# inverse of Fortran
y = np.linspace(0,Ly,Ny)
x = np.linspace(0,Lx,Nx)

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(10, 8))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

im = ax.pcolormesh(x,y,grid,shading='none')

ax.set_xlabel("x direction")
ax.set_ylabel("y direction")

ax.set_title("Grid from {}".format(file_path))

cb = colorbar(im)

fig.tight_layout()

show()
