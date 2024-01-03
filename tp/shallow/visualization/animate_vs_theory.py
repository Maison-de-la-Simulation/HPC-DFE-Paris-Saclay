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
from scipy import optimize


# ______________________________________________________________________________
# solution analytique du probleme Dam break
def onde_detante(x, g, h1):
    return (2 * np.sqrt(g * h1) - 2 * np.sqrt(g * x))

def onde_choc(x, g, h0):
    return ((x - h0) * np.sqrt(g * (x  + h0) / (2 * h0 * x)))

def func_etoile(x, g, h0, h1):
    if(x==0):
        return (0)
    else:
        return(onde_detante(x, g, h1) - onde_choc(x, g, h0))

def fun_dam_break(eta, u, x, t, length, size):

    h0 = 1.0
    h1 = 2.0
    dx = length / size

    g = 9.8
    c1 = np.sqrt(g * h1)

    h_etoile = optimize.fsolve(func_etoile, (h0+h1), args=(g, h1, h0))
    u_etoile = onde_detante(h_etoile, g, h1)
    lambda_etoile = u_etoile - np.sqrt(g * h_etoile)
    s_point = (h_etoile * u_etoile) / (h_etoile - h0)
    for i in range(size):
        dam = (size/2)*dx
        val_x = x[i] - dam
        if(val_x <= - c1 * t):
            u[i] = 0
            eta[i] = h1
        elif(val_x >= - c1 * t and val_x <= lambda_etoile * t):
            u[i] = 2 / 3 * (val_x / t + c1)
            eta[i] = 1 / (9 * g) * (- val_x / t + 2 * c1)**2
        elif(val_x >= lambda_etoile * t and val_x <= s_point * t):
            u[i] = u_etoile
            eta[i] = h_etoile
        elif(val_x >= s_point * t):
            u[i] = 0
            eta[i] = h0
    return eta, u

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
    sys.exit("Please, specify a valid path to diags folder")

# ______________________________________________________________________________
# Get the list of files

file_list = glob.glob("{}/diag_*.bin".format(file_path))

file_list = sorted(file_list)

number_of_files = len(file_list)

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
    length = struct.unpack('d',content[k:k+8])[0] ; k+= 8
    size = struct.unpack('i',content[k:k+4])[0] ; k+= 4

    h = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size
    uh = np.array(struct.unpack('{}d'.format(size),content[k:k + 8*size])) ; k+= 8*size

    x = np.linspace ( 0, length, size )

    dt = 0.1 * length / size

    t = iteration * dt

    true_h = np.zeros(size)
    true_u = np.zeros(size)

    true_h, true_u = fun_dam_break(true_h, true_u, x, t, length, size)

    im0 = ax0.stackplot(x, h)
    im0 = ax0.plot(x, true_h, label="solution")


    ax0.set_xlabel("x")
    ax0.set_ylabel("water height")
    ax0.set_xlim([0, length])
    ax0.set_ylim([0,3])

    ax1.cla()
    ax1.stackplot(x, uh)
    ax1.plot(x, true_h*true_u, label="solution")
    ax1.set_xlabel("x")
    ax1.set_ylabel("mass velocity")

    ax0.set_title("it: {}".format(iteration))
    ax0.legend()
    ax1.legend()

    return im0

# ______________________________________________________________________________
# Figure and plot

fig = figure(figsize=(12, 8))
gs = GridSpec(2, 2)
ax0 = subplot(gs[0,:])
ax1 = subplot(gs[1,:])

animate(0)

nt = len(file_list)

ani = animation.FuncAnimation(fig, animate, np.arange(0, nt),
                              interval=500, blit=False)

fig.tight_layout()

show()
