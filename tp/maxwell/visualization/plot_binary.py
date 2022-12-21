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

nxp = struct.unpack('i',content[k:k+4])[0] ; k+= 4
nyp = struct.unpack('i',content[k:k+4])[0] ; k+= 4

nxd = nxp + 1
nyd = nyp + 1

print(" nx (primal): {}".format(nxp))
print(" ny (primal): {}".format(nyp))

size = nxd*nyp
Ex = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
size = nxp*nyd
Ey = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
size = nxp*nyp
Ez = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size

size = nxp*nyd
Bx = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
size = nxd*nyp
By = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
size = nxd*nyd
Bz = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size

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

Ex_map = np.reshape(Ex, (nxd,nyp))
Ey_map = np.reshape(Ey, (nxp,nyd))
Ez_map = np.reshape(Ez, (nxp,nyp))

Bx_map = np.reshape(Bx, (nxp,nyd))
By_map = np.reshape(By, (nxd,nyp))
Bz_map = np.reshape(Bz, (nxd,nyd))

xp = np.arange(nxp)
yp = np.arange(nyp)

xd = np.arange(nxd)
yd = np.arange(nyd)

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

im00 = ax00.pcolormesh(xd,yp,Ex_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im00)
cb00 = colorbar(im00, ax=ax00)
ax00.set_title("Ex (it: {})".format(iteration))
#im00.set_clim([-1e-6,1e-6])

im01 = ax01.pcolormesh(xp,yd,Ey_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im01)
cb01 = colorbar(im01, ax=ax01)
ax01.set_title("Ey")

im02 = ax02.pcolormesh(xp,yp,Ez_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im02)
cb02 = colorbar(im02, ax=ax02)
ax02.set_title("Ez")

im10 = ax10.pcolormesh(xp,yd,Bx_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im10)
cb10 = colorbar(im10, ax=ax10)
ax10.set_title("Bx")

im11 = ax11.pcolormesh(xd,yp,By_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im11)
cb11 = colorbar(im11, ax=ax11)
ax11.set_title("By")

im12 = ax12.pcolormesh(xd,yd,Bz_map.T,shading='auto',cmap=colormaps[colormap_name])
symetric(im12)
cb12 = colorbar(im12, ax=ax12)
ax12.set_title("Bz")

ax10.set_xlabel("x")
ax11.set_xlabel("x")

ax00.set_ylabel("y")
ax10.set_ylabel("y")

fig.tight_layout()

show()
