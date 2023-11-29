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
times["cores"]       = np.array([4,8, 16, 32])
# Temps dans la boucle en temps
times["loop"] = np.array([150.516261,38.981609,10.206994,2.790699])
# Temps pour les collisions
times["collision"] = np.array([149.820975, 38.585408,9.939499,2.588846])
# Temps dans le pousseur
times["pusher"] = np.array([0.072280, 0.039031,0.023502,0.026518])
# Temps dans les echanges de particules
times["exchange"] = np.array([0.316816, 0.166301,0.106050, 0.058048])
# Temps dans les communications globales
times["global"] = np.array([0.054159, 0.056822, 0.060616, 0.067346])

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

#ax.set_yscale("log")

ax0.set_title("Fig. 2.1 - Strong scaling OpenMP, 1000 particules par patch")

ax0.legend(loc="best",ncol=2)
ax1.legend(loc="best",ncol=2)

fig0.tight_layout()

# ______________________________________________________________________________
# Figure efficacite

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation",marker='o')

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite",marker='o')

# ax.set_ylim([0.,1.1])

ax.set_title("Fig. 2.2 - Strong scaling OpenMP, 1000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig.tight_layout()

# ______________________________________________________________________________
# Figure Part MPI

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='Collision',marker='o')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='Echange particules',marker='o')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='Communications globales',marker='o')

ax.set_title("Fig. 2.3 - Strong scaling OpenMP, 1000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

# _____________________________________________________________________________

show()
