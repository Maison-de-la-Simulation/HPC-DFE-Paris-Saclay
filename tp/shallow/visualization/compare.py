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

file_path_list = []

if len(sys.argv) > 2:
    file_path_list.append(sys.argv[1])
    file_path_list.append(sys.argv[2])
else:
    sys.exit("Please, specify 2 valid file path for comparison")


# ______________________________________________________________________________
# Read the binary file

h_list = []
uh_list = []
x_list = []

for path in file_path_list:

    file = open(path, 'rb')

    content = file.read()

    k = 0
    iteration = struct.unpack('i',content[k:k+4])[0] ; k+= 4
    length = struct.unpack('d',content[k:k+8])[0] ; k+= 8
    size = struct.unpack('i',content[k:k+4])[0] ; k+= 4

    h_list.append(np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size]))) ; k+= 8*size
    uh_list.append(np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size]))) ; k+= 8*size

    x_list.append(np.linspace ( 0, length, size ))

error_h = np.abs(h_list[0] - h_list[1])
error_uh = np.abs(uh_list[0] - uh_list[1]) 

# print min, max and average error

print(" Error h: min: {} max: {} average: {}".format(np.min(error_h), np.max(error_h), np.mean(error_h)))
print(" Error uh: min: {} max: {} average: {}".format(np.min(error_uh), np.max(error_uh), np.mean(error_uh)))

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 8))
gs = GridSpec(2, 2)
ax0 = subplot(gs[0,:])
ax1 = subplot(gs[1,:])

im0 = ax0.plot(x_list[0], error_h, 'b', label='h')

ax0.set_xlabel("x")
ax0.set_ylabel("Error water height")
ax0.set_xlim([0, length])
ax0.set_ylim([0,3])

ax1.cla()
ax1.plot(x_list[0], error_uh, 'b', label='uh')
ax1.set_xlabel("x")
ax1.set_ylabel("Error mass velocity")

ax0.set_title("it: {}".format(iteration))

fig.tight_layout()

show()
