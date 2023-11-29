# ______________________________________________________________________________
#
# Read and plot the 3 species
#
# ______________________________________________________________________________

import sys
import struct
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
    sys.exit("Please, specify a valid path to a binary file as a first command line argument.")

# ______________________________________________________________________________
# Read the binary file

file = open(file_path, 'rb')

content = file.read()

k = 0

iteration = struct.unpack('i',content[k:k+4])[0] ; k+= 4

print(" Iteration: {}".format(iteration))

nx = struct.unpack('i',content[k:k+4])[0] ; k+= 4
ny = struct.unpack('i',content[k:k+4])[0] ; k+= 4

print(" nx: {}".format(nx))
print(" ny: {}".format(ny))

plants = np.array(struct.unpack('{}d'.format(nx*ny),content[k:k + 8*nx*ny])) ; k+= 8*nx*ny
rabbits = np.array(struct.unpack('{}i'.format(nx*ny),content[k:k + 4*nx*ny])) ; k+= 4*nx*ny
foxes = np.array(struct.unpack('{}i'.format(nx*ny),content[k:k + 4*nx*ny])) ; k+= 4*nx*ny

plants_map = np.reshape(plants, (ny,nx))
rabbits_map = np.reshape(rabbits, (ny,nx))
foxes_map = np.reshape(foxes, (ny,nx))

x = np.arange(nx)
y = np.arange(ny)

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(14, 5))

gs = GridSpec(4, 3)
ax0 = subplot(gs[:,0])
ax1 = subplot(gs[:,1])
ax2 = subplot(gs[:,2])

im0 = ax0.pcolormesh(x,y,plants_map,shading='auto',cmap=get_cmap('Greens'))

cb0 = colorbar(im0, ax=ax0)

im1 = ax1.pcolormesh(x,y,rabbits_map,shading='auto',cmap=get_cmap('Greys'))

cb1 = colorbar(im1, ax=ax1)

im2 = ax2.pcolormesh(x,y,foxes_map,shading='auto',cmap=get_cmap('Oranges'))

cb2 = colorbar(im2, ax=ax2)

ax0.set_xlabel("x")
ax1.set_xlabel("x")
ax2.set_xlabel("x")
ax0.set_ylabel("y")

ax0.set_title("Plants - it: {}".format(iteration))
ax1.set_title("Rabbit".format(iteration))
ax2.set_title("foxes".format(iteration))

fig.tight_layout()

show()
