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

times = {}

# Nombre de processus
times["cores"]       = np.array([8,16,24,32,64])
# Temps dans la boucle en temps
times["loop"] = np.array([0.86520, 0.935701, 1.020537, 1.06815, 1.219218])
# Temps pour les collisions
times["collision"] = np.array([0.564491, 0.565023, 0.565586, 0.567919, 0.567763])
# Temps dans les echanges de particules
times["exchange"] = np.array([0.265890,0.274040,0.364613,0.312903,0.364556])
# Temps dans les communications globales
times["exchange"] = np.array([0.265890,0.274040,0.364613,0.312903,0.364556])

# Calcul de l'efficacite pour une scalabilite faible
efficiency = times["loop"][:] / times["loop"][0]


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

ax.plot(times["cores"],times["loop"],lw=2,label="Boucle de calcul")
ax.plot(times["cores"],times["collision"],lw=2,label="Collision")
ax.plot(times["cores"],times["exchange"],lw=2,label="Temps echange particules")
ax.plot(times["cores"],times["global"],lw=2,label="Temps echange particules")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")

ax.legend(loc="best")

fig.tight_layout()

# ______________________________________________________________________________
# Figure efficacite

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

#ax.plot(processus,efficiency,lw=2,label="Simulation")

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

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

# ax.plot(times["cores"],computation_times/simulation_times*100.,lw=2,color='C1',label='part equation onde')
# ax.plot(times["cores"],mpi_times/simulation_times*100.,lw=2,color='C2',label='part MPI')

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

# _____________________________________________________________________________

show()
