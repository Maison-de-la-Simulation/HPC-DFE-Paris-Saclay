# ______________________________________________________________________________
#
# Read and plot the grid at a specific iteration
#
# ______________________________________________________________________________

import sys
import h5py
import numpy as np
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
    sys.exit("Please, specify a valid path to a charge hdf5 file as a first command line argument.")


# ______________________________________________________________________________
# Read the hdf5 file

try:
    f = h5py.File(file_path, "r")
except IOError:
    sys.exit("The specified hdf5 file is not valid.")

grid = np.array(f['data'])

shape = list(np.shape(grid))

# inverse of Fortran
y = np.linspace(0,shape[0],shape[0])
x = np.linspace(0,shape[1],shape[1])

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(10, 8))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

im = ax.pcolormesh(x,y,grid,shading='none')

ax.set_xlabel("Vertice index in the x direction")
ax.set_ylabel("Vertice index in the y direction")

ax.set_title("Diffusion grid")

cb = colorbar(im)

fig.tight_layout()

show()
