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
times["cores"]       = np.array([4,8,16,32,64])
# Temps dans la boucle en temps
times["loop"] = np.array([148.410497,37.865224,10.297101,3.131366,1.035853])
# Temps pour les collisions
times["collision"] = np.array([144.869179,36.129406,9.030512,2.272468,0.567437])
# Temps dans le pousseur
times["pusher"] = np.array([0.067916,0.034055,0.017134,0.008759,0.004411])
# Temps dans les echanges de particules
times["exchange"] = np.array([3.215714,1.562653,1.049943,0.590709,0.278090])
# Temps dans les communications globales
times["global"] = np.array([0.013876,0.016409,0.137428,0.227660, 0.169374])


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

ax0.plot(times["cores"],times["loop"],lw=2,label="Boucle de calcul")
ax0.plot(times["cores"],times["collision"],lw=2,label="Collision")
ax1.plot(times["cores"],times["exchange"],lw=2,label="Echange particules")
ax1.plot(times["cores"],times["global"],lw=2,label="Communications globales")
ax1.plot(times["cores"],times["pusher"],lw=2,label="Equation du mouvement")

ax0.set_xlabel("Nombre de processus")
ax0.set_ylabel("Temps (s)")

ax1.set_xlabel("Nombre de processus")
ax1.set_ylabel("Temps (s)")

ax0.set_title("Fig. 5.1 - Strong scaling MPI, 1000 particules par patch")

#ax.set_yscale("log")

ax0.legend(loc="best",ncol=2)
ax1.legend(loc="best",ncol=2)

fig0.tight_layout()

fig0.savefig("../../../support/materiel/marble_mpi_strong_scaling_ppp1000_time.png")

# ______________________________________________________________________________
# Figure efficacite

fig1 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation")

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_title("Fig. 5.2 - Strong scaling MPI, 1000 particules par patch")

# ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig1.tight_layout()

fig1.savefig("../../../support/materiel/marble_mpi_strong_scaling_ppp1000_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='Collision')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='Echange particules')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='Communications globales')

ax.set_title("Fig. 5.3 - Strong scaling MPI, 1000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

fig2.savefig("../../../support/materiel/marble_mpi_strong_scaling_ppp1000_time.png")

# _____________________________________________________________________________

show()
