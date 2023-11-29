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
times["cores"]       = np.array([1, 2, 4, 8,16,24,32,64])
# Temps dans la boucle en temps
times["loop"] = np.array([0.616910, 0.642110, 0.785144, 0.86520, 0.935701, 1.020537, 1.06815, 1.219218])
# Temps pour les collisions
times["collision"] = np.array([0.562996, 0.566060, 0.563235, 0.564491, 0.565023, 0.565586, 0.567919, 0.567763])
# Temps dans les echanges de particules
times["exchange"] = np.array([0.031663, 0.051521, 0.195062, 0.265890, 0.274040, 0.364613, 0.312903, 0.364556])
# Temps dans les communications globales
times["global"] = np.array([0.001389, 0.003636, 0.005965, 0.013941, 0.075780, 0.069431, 0.166404, 0.265865])


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

fig0 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["loop"],lw=2,label="Boucle de calcul",marker='o')
ax.plot(times["cores"],times["collision"],lw=2,label="Collision",marker='o')
ax.plot(times["cores"],times["exchange"],lw=2,label="Echange particules",marker='o')
ax.plot(times["cores"],times["global"],lw=2,label="Communications globales",marker='o')

ax.set_title("Fig. 4.1 - Weak scaling MPI, 500 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")



#ax.set_yscale("log")

ax.legend(loc="best",ncol=2)

fig0.tight_layout()

fig0.savefig("../../../support/materiel/marble_mpi_weak_scaling_ppp500_time.png")

# ______________________________________________________________________________
# Figure efficacite

fig1 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation",marker='o')

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_title("Fig. 4.2 - Weak scaling MPI, 500 particules par patch")

ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig1.tight_layout()

fig1.savefig("../../../support/materiel/marble_mpi_weak_scaling_ppp500_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='Collision',marker='o')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='Echange particules',marker='o')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='Communications globales',marker='o')

ax.set_title("Fig. 4.3 - Weak scaling MPI, 500 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

fig2.savefig("../../../support/materiel/marble_mpi_weak_scaling_ppp500_part.png")

# _____________________________________________________________________________

show()
