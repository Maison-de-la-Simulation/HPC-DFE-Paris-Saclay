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

file_path_1 = ""
file_path_2 = ""

if len(sys.argv) > 2:
    file_path_1 = sys.argv[1]
    file_path_2 = sys.argv[2]
else:
    sys.exit("Please, specify a valid path to a charge hdf5 file as a first command line argument.")

# ______________________________________________________________________________
# Get the list of files

file_list_1 = glob.glob("{}/grid_*.dat".format(file_path_1))
file_list_2 = glob.glob("{}/grid_*.dat".format(file_path_2))

file_list_1 = sorted(file_list_1)
file_list_2 = sorted(file_list_2)

print("")
print("{}".format(file_list_1))

print("")
print("{}".format(file_list_2))

if (len(file_list_1) != len(file_list_2)):
    sys.exit("The directories do not have the same number of files.")

print("")
for i in range(len(file_list_1)):

    print("Analyse number {}:".format(i))

    file_1 = open(file_list_1[i], 'r')
    file_2 = open(file_list_2[i], 'r')

    content_1 = file_1.read()
    content_2 = file_2.read()

    Nx_1 = struct.unpack('i',content_1[0:4])[0]
    Ny_1 = struct.unpack('i',content_1[4:8])[0]
    Lx_1 = struct.unpack('d',content_1[8:16])[0]
    Ly_1 = struct.unpack('d',content_1[16:24])[0]
    
    Nx_2 = struct.unpack('i',content_2[0:4])[0]
    Ny_2 = struct.unpack('i',content_2[4:8])[0]
    Lx_2 = struct.unpack('d',content_2[8:16])[0]
    Ly_2 = struct.unpack('d',content_2[16:24])[0]

    grid_1 = np.zeros([Ny_1,Nx_1])
    grid_2 = np.zeros([Ny_1,Nx_1])

    for iy in range(Ny_1):
        grid_1[iy,:] = np.array(struct.unpack('{}d'.format(Nx_1),content_1[24 + 8*iy*Nx_1:24 + 8*(iy+1)*Nx_1]))
    for iy in range(Ny_2):
        grid_2[iy,:] = np.array(struct.unpack('{}d'.format(Nx_2),content_2[24 + 8*iy*Nx_2:24 + 8*(iy+1)*Nx_2]))
        
    error = np.abs(grid_1 - grid_2)
    error_rel = np.abs((grid_1[grid_1<>0] - grid_2[grid_1<>0]) / grid_1[grid_1<>0])
    error_max = np.max(error)
    error_rel_max = np.max(error_rel)
    error_sum = np.sum(error)
    print(" - maximal error: {}".format(error_max))
    print(" - maximal relative error: {}".format(error_rel_max))
    print(" - sum error: {}".format(error_sum))
