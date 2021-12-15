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
times["total"] = np.array([148.410497,37.865224,10.297101,3.131366,1.035853])
# Temps dans la boucle en temps
times["calcul"] = np.array([148.410497,37.865224,10.297101,3.131366,1.035853])
# Temps pour les collisions
times["point"] = np.array([144.869179,36.129406,9.030512,2.272468,0.567437])
# Temps dans le pousseur
times["global"] = np.array([0.067916,0.034055,0.017134,0.008759,0.004411])

# Calcul de l'efficacite pour une scalabilite faible
times["efficiency"] = times["total"][0] * times["cores"][0] / (times["total"][:] * times["cores"][:])

print(times["total"][:] * times["cores"][:])

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

ax0.plot(times["cores"],times["total"],lw=2,label="Boucle de calcul",marker='o')
ax0.plot(times["cores"],times["calcul"],lw=2,label="Calcul",marker='o')
ax1.plot(times["cores"],times["point"],lw=2,label="Com. point a point",marker='o')
ax1.plot(times["cores"],times["global"],lw=2,label="Com. globales",marker='o')

ax0.set_xlabel("Nombre de processus")
ax0.set_ylabel("Temps (s)")

ax1.set_xlabel("Nombre de processus")
ax1.set_ylabel("Temps (s)")

ax0.set_title("Fig. 4.4 - Strong scaling MPI")

#ax.set_yscale("log")

ax0.legend(loc="best",ncol=2)
ax1.legend(loc="best",ncol=2)

fig0.tight_layout()

fig0.savefig("../../../support/materiel/life_mpi_strong_scaling_time.png")

# ______________________________________________________________________________
# Figure efficacite

fig1 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,marker='o',label="Simulation")

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_title("Fig. 4.5 - Strong scaling MPI")

# ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacite")

ax.legend(loc="best")

fig1.tight_layout()

fig1.savefig("../../../support/materiel/life_mpi_strong_scaling_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["calcul"]/times["total"]*100.,lw=2,marker='o',color='C1',label='Collision')
ax.plot(times["cores"],times["point"]/times["total"]*100.,lw=2,marker='o',color='C2',label='Echange particules')
ax.plot(times["cores"],times["global"]/times["total"]*100.,lw=2,marker='o',color='C3',label='Communications globales')

ax.set_title("Fig. 4.6 - Strong scaling MPI")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

fig2.savefig("../../../support/materiel/life_mpi_strong_scaling_part.png")

# _____________________________________________________________________________

show()
