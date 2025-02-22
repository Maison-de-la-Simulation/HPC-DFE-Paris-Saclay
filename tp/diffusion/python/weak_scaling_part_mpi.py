# ______________________________________________________________________________
#
# Script d'aide au trace du temps pour les etudes de scalabilite
#
# ______________________________________________________________________________

import numpy as np
from matplotlib import *
from matplotlib.pyplot import *

# ______________________________________________________________________________
# Les donnees

# Mettre ici le nombre de processus successif utilise
processus        = np.array([])
# Mettre ici les temps de simulations correspondants
simulation_times = np.array([])
# Mettre ici les temps MPI
mpi_times        = np.array([])

# ______________________________________________________________________________
# RCParams - pour ameliorer le rendu de la figure
# https://matplotlib.org/users/customizing.html

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
# Figure et trace

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(processus,mpi_times/simulation_times*100.,lw=2)

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part MPI (%)")

ax.legend(loc="best")

fig.tight_layout()

show()
