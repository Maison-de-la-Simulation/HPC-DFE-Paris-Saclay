# ______________________________________________________________________________
#
# Read and plot the grids at a specific iteration
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

path = ""

if len(sys.argv) > 1:
    path = sys.argv[1]
    iteration = int(sys.argv[2])
else:
    sys.exit("Please, specify a valid path to a binary file as a first command line argument.")

# ______________________________________________________________________________
# Read the binary file

def read_file(file_path):

    file = open(file_path, 'rb')

    content = file.read()

    k = 4

    # iteration_file = struct.unpack('i',content[k:k+4])[0] ; k+= 4

    print(" Iteration: {}".format(iteration))

    nx = struct.unpack('i',content[k:k+4])[0] ; k+= 4
    ny = struct.unpack('i',content[k:k+4])[0] ; k+= 4

    size = nx*ny
    array = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size

    map = np.reshape(array, (nx,ny))

    x = np.arange(nx)
    y = np.arange(ny)

    return map, x, y

Ex_map, Ex_x, Ex_y = read_file("{}/diag_Ex_{:05d}.bin".format(path,iteration))
Ey_map, Ey_x, Ey_y = read_file("{}/diag_Ey_{:05d}.bin".format(path,iteration))
Ez_map, Ez_x, Ez_y = read_file("{}/diag_Ez_{:05d}.bin".format(path,iteration))
Bx_map, Bx_x, Bx_y = read_file("{}/diag_Bx_{:05d}.bin".format(path,iteration))
By_map, By_x, By_y = read_file("{}/diag_By_{:05d}.bin".format(path,iteration))
Bz_map, Bz_x, Bz_y = read_file("{}/diag_Bz_{:05d}.bin".format(path,iteration))


# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 6))

gs = GridSpec(4, 6)
ax00 = subplot(gs[0:2,0:2])
ax01 = subplot(gs[0:2,2:4])
ax02 = subplot(gs[0:2,4:6])
ax10 = subplot(gs[2:4,0:2])
ax11 = subplot(gs[2:4,2:4])
ax12 = subplot(gs[2:4,4:6])

colormap_name = "RdBu"

def symetric(im):
    clim = np.array(im.get_clim())
    max = 0.8*np.max(np.absolute(clim))
    im.set_clim([-max,max])

im00 = ax00.pcolormesh(Ex_x,Ex_y,Ex_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im00)
cb00 = colorbar(im00, ax=ax00)
ax00.set_title("Ex (it: {})".format(iteration))
#im00.set_clim([-1e-6,1e-6])

im01 = ax01.pcolormesh(Ey_x,Ey_y,Ey_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im01)
cb01 = colorbar(im01, ax=ax01)
ax01.set_title("Ey")

im02 = ax02.pcolormesh(Ez_x,Ez_y,Ez_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im02)
cb02 = colorbar(im02, ax=ax02)
ax02.set_title("Ez")

im10 = ax10.pcolormesh(Bx_x,Bx_y,Bx_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im10)
cb10 = colorbar(im10, ax=ax10)
ax10.set_title("Bx")

im11 = ax11.pcolormesh(By_x,By_y,By_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im11)
cb11 = colorbar(im11, ax=ax11)
ax11.set_title("By")

im12 = ax12.pcolormesh(Bz_x,Bz_y,Bz_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im12)
cb12 = colorbar(im12, ax=ax12)
ax12.set_title("Bz")

# ax10.set_xlabel("x")
# ax11.set_xlabel("x")

# ax00.set_ylabel("y")
# ax10.set_ylabel("y")

fig.tight_layout()

show()
