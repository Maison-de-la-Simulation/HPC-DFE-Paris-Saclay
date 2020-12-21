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
times["cores"]       = np.array([1,2,4,8,16,24,32,40,64])
# Temps dans la boucle en temps
times["loop"] = np.array([9.102406,9.287328,9.592549,9.930410,10.322814,10.406666,10.530968,11.372397,11.494437])
# Temps pour les collisions
times["collision"] = np.array([8.917669,8.916775,8.901272,8.920579,8.933036,8.954799,8.953011,8.975957,8.960590])
# Temps dans le pousseur
times["pusher"] = np.array([0.017165,0.017201,0.017129,0.017264,0.017221,0.017159,0.017248,0.017248 ,0.017298])
# Temps dans les echanges de particules
times["exchange"] = np.array([0.100065,0.286112,0.603172,0.913153,1.180107,1.196473,1.232309,1.625429,1.727475])
# Temps dans les communications globales
times["global"] = np.array([0.003137,0.004929,0.008800,0.017287,0.130249,0.176014,0.266086,0.691307,0.726807])



# Calcul de l'efficacite pour une scalabilite faible
times["efficiency"] = times["loop"][0] / times["loop"][:]


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

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='part collision')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='part echange particules')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='part communications globales')

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

# _____________________________________________________________________________

show()
