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
times["cores"]       = np.array([1,2,4,8,16,32])
# Temps dans la boucle en temps
times["loop"] = np.array([75.971588,38.237486,19.541912,10.006992,5.181155,2.790699])
# Temps pour les collisions
times["collision"] = np.array([73.377270,36.915792,18.817079,9.595611,4.918988,2.588846])
# Temps dans le pousseur
times["pusher"] = np.array([0.279227,0.147323,0.076493,0.041538,0.025976,0.026518])
# Temps dans les echanges de particules
times["exchange"] = np.array([1.277869,0.635983,0.330567,0.172132,0.092681,0.058048])
# Temps dans les communications globales
times["global"] = np.array([0.045122,0.041882,0.060783,0.062672,0.065473,0.067346])

# Calcul de l'efficacite pour une scalabilite faible
times["efficiency"] = times["loop"][0] * times["cores"][0] / (times["loop"][:] * times["cores"][:])








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

ax0.set_title("Fig. 3.1 - Strong scaling OpenMP, 1000 particules par patch")

ax0.legend(loc="best",ncol=2)
ax1.legend(loc="best",ncol=2)

fig0.tight_layout()

fig0.savefig("../../../support/materiel/marble_omp_strong_scaling_ppp1000_mp_time.png")

# ______________________________________________________________________________
# Figure efficacite

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation",marker='o')

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite",marker='o')

# ax.set_ylim([0.,1.1])

ax.set_title("Fig. 3.2 - Strong scaling OpenMP, 1000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig.tight_layout()

fig.savefig("../../../support/materiel/marble_omp_strong_scaling_ppp1000_mp_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["collision"]/times["loop"]*100.,lw=2,color='C1',label='Collision',marker='o')
ax.plot(times["cores"],times["exchange"]/times["loop"]*100.,lw=2,color='C2',label='Echange particules',marker='o')
ax.plot(times["cores"],times["global"]/times["loop"]*100.,lw=2,color='C3',label='Communications globales',marker='o')

ax.set_title("Fig. 3.3 - Strong scaling OpenMP, 1000 particules par patch")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig.tight_layout()

fig.savefig("../../../support/materiel/marble_omp_strong_scaling_ppp1000_mp_part.png")

# _____________________________________________________________________________

show()
