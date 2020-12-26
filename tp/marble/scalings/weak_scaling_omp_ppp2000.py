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
times["cores"]       = np.array([1,2,4,8,16,20,32,40])
# Temps dans la boucle en temps
times["loop"] = np.array([9.259840,9.419337,9.576945,9.820962,9.967579, (10.157838+10.140684)/2, (11.001242+10.774656)/2,(10.364162+10.526973)/2])
# Temps pour les collisions
times["collision"] = np.array([9.101788,9.251330,9.398662,9.622947,9.737170, (9.905278+9.891084)/2, (10.624268+10.426451)/2,(9.973284+10.138378)/2])
# Temps dans le pousseur
times["pusher"] = np.array([0.017323,0.018130,0.018521,0.019021,0.020274, (0.023477+0.020972)/2, (0.031453+0.036474)/2,(0.038243+0.038078)/2])
# Temps dans les echanges de particules
times["exchange"] = np.array([0.077495,0.079330,0.081041,0.084134,0.086661, (0.088879+0.090321)/2, (0.106096+0.101479)/2, (0.100227+0.101322)/2])
# Temps dans les communications globales
times["global"] = np.array([0.002301,0.005574,0.012189,0.025551,0.051537, (0.064872+0.064722)/2, (0.118343+0.119140)/2, (0.154247+0.156822)/2])







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

rcParams['axes.grid'] = True

# ______________________________________________________________________________
# Figure temps de simulation

fig0 = figure(figsize=(12, 8))
gs = GridSpec(2, 2)
ax0 = subplot(gs[0,:])
ax1 = subplot(gs[1,:])

ax0.plot(times["cores"],times["loop"],lw=2,label="Boucle de calcul",marker='o',color='C0')
ax0.plot(times["cores"],times["collision"],lw=2,label="Collision",marker='o',color='C1')
ax1.plot(times["cores"],times["exchange"],lw=2,label="Echange particules",marker='o',color='C2')
ax1.plot(times["cores"],times["global"],lw=2,label="Communications globales",marker='o',color='C3')
ax1.plot(times["cores"],times["pusher"],lw=2,label="Equation du mouvement",marker='o',color='C4')

ax0.set_xlabel("Nombre de processus")
ax0.set_ylabel("Temps (s)")

ax1.set_xlabel("Nombre de processus")
ax1.set_ylabel("Temps (s)")

ax0.set_title("Fig. 1.1 - Weak scaling OpenMP, 2000 particules par patch")

#ax.set_yscale("log")

ax0.legend(loc="best",ncol=2)
ax1.legend(loc="best",ncol=2)

fig0.tight_layout()

# ______________________________________________________________________________
# Figure efficacite

fig1 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation")

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_ylim([0.,1.1])

ax.set_title("Fig. 1.2 - Weak scaling OpenMP, 2000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig1.tight_layout()

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='part collision')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='part echange particules')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='part communications globales')

ax.set_title("Fig. 1.3 - Weak scaling OpenMP, 2000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

# _____________________________________________________________________________

show()
