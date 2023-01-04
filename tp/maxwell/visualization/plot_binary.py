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

print(" nx : {}".format(nx))
print(" ny : {}".format(ny))

size = nx*ny
Field = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size


# Ez_map = np.zeros([nyp, nxp])
# for iy in range(nyp):
#     for ix in range(nxp):
#         Ez_map[iy,ix] = Ez[iy*nxp+ix]

# Ex_map = np.reshape(Ex, (nyp,nxd))
# Ey_map = np.reshape(Ey, (nyd,nxp))
# Ez_map = np.reshape(Ez, (nyp,nxp))

# Bx_map = np.reshape(Bx, (nyd,nxp))
# By_map = np.reshape(By, (nyp,nxd))
# Bz_map = np.reshape(Bz, (nyd,nxd))

Field_map = np.reshape(Field, (nx,ny))

x = np.arange(nx)
y = np.arange(ny)


# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(8, 6))

gs = GridSpec(4, 6)
ax00 = subplot(gs[:,:])


colormap_name = "RdBu"

def symetric(im):
    clim = np.array(im.get_clim())
    max = 0.8*np.max(np.absolute(clim))
    im.set_clim([-max,max])

im00 = ax00.pcolormesh(x,y,Field_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im00)
cb00 = colorbar(im00, ax=ax00)
ax00.set_title("{}".format(file_path))
#im00.set_clim([-1e-6,1e-6])

ax00.set_xlabel("x")
ax00.set_ylabel("y")

fig.tight_layout()

show()
