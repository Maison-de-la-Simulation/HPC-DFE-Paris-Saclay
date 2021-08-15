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
    sys.exit("Please, specify a valid path to a charge hdf5 file as a first command line argument.")

# ______________________________________________________________________________
# Get the list of files

file_list = glob.glob("{}/diag_*.bin".format(file_path))

file_list = sorted(file_list)

#print(file_list)


# ______________________________________________________________________________
# Animation function

def animate(i):

    ax0.cla()
    ax1.cla()

    file = open(file_list[i], 'rb')

    content = file.read()

    k = 0
    iteration = struct.unpack('i',content[k:k+4])[0] ; k+= 4
    nx = struct.unpack('i',content[k:k+4])[0] ; k+= 4
    ny = struct.unpack('i',content[k:k+4])[0] ; k+= 4

    plants = np.array(struct.unpack('{}d'.format(nx*ny),content[k:k + 8*nx*ny])) ; k+= 8*nx*ny
    rabbits = np.array(struct.unpack('{}i'.format(nx*ny),content[k:k + 4*nx*ny])) ; k+= 4*nx*ny
    foxes = np.array(struct.unpack('{}i'.format(nx*ny),content[k:k + 4*nx*ny])) ; k+= 4*nx*ny

    plants_map = np.reshape(plants, (ny,nx))
    rabbits_map = np.reshape(rabbits, (ny,nx))
    foxes_map = np.reshape(foxes, (ny,nx))

    x = np.arange(nx)
    y = np.arange(ny)

    im0 = ax0.pcolormesh(x,y,plants_map,shading='auto',cmap=get_cmap('Greens'))
    im0.set_clim([0,10])
    #cb0 = colorbar(im0, ax=ax0)
    im1 = ax1.pcolormesh(x,y,rabbits_map,shading='auto',cmap=get_cmap('Greys'))
    im1.set_clim([0,20])

    im2 = ax2.pcolormesh(x,y,foxes_map,shading='auto',cmap=get_cmap('Oranges'))
    im2.set_clim([0,20])

    ax0.set_title("Plants, it: {} ({})".format(i,np.sum(plants)))
    ax1.set_title("Rabbits ({})".format(np.sum(rabbits)))
    ax2.set_title("foxes ({})".format(np.sum(foxes)))

    ax0.set_xlabel("x")
    ax1.set_xlabel("x")
    ax2.set_xlabel("x")
    ax0.set_ylabel("y")

    return im0

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(14, 6))
gs = GridSpec(4, 3)
ax0 = subplot(gs[:,0])
ax1 = subplot(gs[:,1])
ax2 = subplot(gs[:,2])

animate(0)

nt = len(file_list)

ani = animation.FuncAnimation(fig, animate, np.arange(0, nt),
                              interval=500, blit=False)

fig.tight_layout()

show()
