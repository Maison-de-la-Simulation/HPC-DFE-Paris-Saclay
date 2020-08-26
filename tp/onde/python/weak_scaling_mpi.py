# ______________________________________________________________________________
#
# Script d'aide au trace du temps pour les etudes de scalabilite faible
# pour le programme MPI
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
# Mettre ici les temps de calcul dans l'equation d'onde
computation_times = np.array([])
# Mettre ici les temps dans MPI (cellules fantomes)
mpi_times = np.array([ ])
# Calcul de l'efficacite pour une scalabilite faible
efficiency = ... mettre ici la formule ...


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
# Figure temps de simulation

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(processus,simulation_times,lw=2,label="Temps de simulation total")
ax.plot(processus,computation_times,lw=2,label="Temps de calcul")
ax.plot(processus,mpi_times,lw=2,label="Temps dans les echanges")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")

ax.legend(loc="best")

fig.tight_layout()

# ______________________________________________________________________________
# Figure efficacite

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(processus,efficiency,lw=2,label="Simulation")

ax.plot([processus.min(), processus.max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig.tight_layout()

# ______________________________________________________________________________
# Figure Part MPI

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(processus,computation_times/simulation_times*100.,lw=2,color='C1',label='part equation onde')
ax.plot(processus,mpi_times/simulation_times*100.,lw=2,color='C2',label='part MPI')

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

# _____________________________________________________________________________

show()
