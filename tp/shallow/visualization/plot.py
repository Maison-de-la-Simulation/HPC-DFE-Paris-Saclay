# ______________________________________________________________________________
#
# Read and generate an animation of the grid for all iterations
#
# ______________________________________________________________________________

import sys
import glob
import numpy as np
import struct
from matplotlib import *
from matplotlib.pyplot import *
import matplotlib.animation as animation

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
    sys.exit("Please, specify a valid path to a diag file")


# ______________________________________________________________________________
# Read the binary file

file = open(file_path, 'rb')

content = file.read()

k = 0
iteration = struct.unpack('i',content[k:k+4])[0] ; k+= 4
length = struct.unpack('d',content[k:k+8])[0] ; k+= 8
size = struct.unpack('i',content[k:k+4])[0] ; k+= 4

h = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
uh = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size

x = np.linspace ( 0, length, size )


# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 8))
gs = GridSpec(2, 2)
ax0 = subplot(gs[0,:])
ax1 = subplot(gs[1,:])

im0 = ax0.stackplot(x, h)

ax0.set_xlabel("x")
ax0.set_ylabel("water height")
ax0.set_xlim([0, length])
ax0.set_ylim([0,3])

ax1.cla()
ax1.stackplot(x, uh)
ax1.set_xlabel("x")
ax1.set_ylabel("mass velocity")

ax0.set_title("it: {}".format(iteration))

fig.tight_layout()

show()
