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
times["cores"]       = np.array([4,8,16,64])
# Temps dans la boucle en temps
times["loop"] = np.array([148.410497,37.865224,10.297101,1.035853])
# Temps pour les collisions
times["collision"] = np.array([144.869179,36.129406,9.030512,0.567437])
# Temps dans le pousseur
times["pusher"] = np.array([0.067916,0.034055,0.017134,0.004411])
# Temps dans les echanges de particules
times["exchange"] = np.array([3.215714,1.562653,1.049943,0.278090])
# Temps dans les communications globales
times["global"] = np.array([0.013876,0.016409,0.137428,0.169374])


# Calcul de l'efficacite pour une scalabilite faible
times["efficiency"] = times["loop"][0] * times["cores"][0] / (times["loop"][:] * times["cores"][:])

print(times["loop"][:] * times["cores"][:])

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
ax.plot(times["cores"],times["exchange"],lw=2,label="Echange particules")
ax.plot(times["cores"],times["global"],lw=2,label="Communications globales")
ax.plot(times["cores"],times["pusher"],lw=2,label="Equation du mouvement")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")

#ax.set_yscale("log")

ax.legend(loc="best",ncol=2)

fig.tight_layout()

# ______________________________________________________________________________
# Figure efficacite

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation")

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

# ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig.tight_layout()

# ______________________________________________________________________________
# Figure Part MPI

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='Collision')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='Echange particules')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='Communications globales')

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

# _____________________________________________________________________________

show()