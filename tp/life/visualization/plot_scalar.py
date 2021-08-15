# ______________________________________________________________________________
#
# Read and plot the scalars
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

number_of_iterations = struct.unpack('i',content[k:k+4])[0] ; k+= 4

sum_plants_before_rabbits = np.array(struct.unpack('{}d'.format(number_of_iterations),content[k:k + 8*number_of_iterations])) ; k+= 8*number_of_iterations
sum_plants_after_rabbits = np.array(struct.unpack('{}d'.format(number_of_iterations),content[k:k + 8*number_of_iterations])) ; k+= 8*number_of_iterations
sum_rabbits_before_foxes = np.array(struct.unpack('{}d'.format(number_of_iterations),content[k:k + 8*number_of_iterations])) ; k+= 8*number_of_iterations
sum_foxes = np.array(struct.unpack('{}d'.format(number_of_iterations),content[k:k + 8*number_of_iterations])) ; k+= 8*number_of_iterations

iterations = np.arange(number_of_iterations)

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 6))

gs = GridSpec(4, 3)
ax0 = subplot(gs[:,:])

ax0.plot(iterations,sum_plants_before_rabbits,label='plants before rabbits',color='green')
ax0.plot(iterations,sum_plants_after_rabbits,label='plants after rabbits')
ax0.plot(iterations,sum_rabbits_before_foxes,label='rabbits before foxes',color='grey')
ax0.plot(iterations,sum_foxes,label='foxes',color='orange')

ax0.set_xlabel('iterations')
ax0.legend(loc='best')

fig.tight_layout()

show()
