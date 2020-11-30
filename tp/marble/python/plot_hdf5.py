# ______________________________________________________________________________
#
# Read and plot the particles at a specific iteration
#
# ______________________________________________________________________________

import sys
import h5py
import numpy as np
from matplotlib import *
from matplotlib.pyplot import *
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
    sys.exit("Please, specify a valid path to a hdf5 file as a first command line argument.")

# ______________________________________________________________________________
# Read the hdf5 file

try:
    f = h5py.File(file_path, "r")
except IOError:
    sys.exit("The specified hdf5 file is not valid.")

mass = np.array(f['mass'])
x = np.array(f['x'])
y = np.array(f['y'])
z = np.array(f['z'])
vx = np.array(f['vx'])
vy = np.array(f['vy'])
vz = np.array(f['vz'])

energy = 0.5 * mass * (vx*vx + vy*vy + vz*vz)

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 8))
ax = fig.gca(projection='3d')

im = ax.scatter(x, y, z, c=energy, marker='o', alpha = 1) # s = np.sqrt(radius)
cb = colorbar(im)
cb.set_label("Energy")

ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_zlabel("z")

ax.set_title("Particles from {}".format(file_path))

fig.tight_layout()

show()
