²# ______________________________________________________________________________
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
times["cores"]       = np.array([1, 2, 4, 8,16,32,48,64,96])
# Temps dans la boucle en temps
times["total"] = np.array([7.723384, 8.126372, 8.549324, 9.095991, 9.765752, 11.095566, 12.306443, 13.321309, 15.470952])
# Temps dans la boucle en temps
times["calcul"] = np.array([7.723384, 8.126372, 8.549324, 9.095991, 9.765752, 11.095566, 12.306443, 13.321309, 15.470952])
# Temps pour les collisions
times["point"] = np.array([0.103110, 0.459180, 0.859833, 1.337354, 1.832974, 1.969981, 4.264167, 5.369361, 7.530788])
# Temps dans les echanges de particules
times["global"] = np.array([0.001656, 0.024779, 0.020225, 0.097196, 0.240307, 1.432899, 0.324243, 0.411199, 0.559528])


# Calcul de l'efficacite pour une scalabilite faible
times["efficiency"] = times["total"][0] / times["total"][:]

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

ax.plot(times["cores"],times["total"],lw=2,label="Boucle en temps",marker='o')
ax.plot(times["cores"],times["calcul"],lw=2,label="Calcul",marker='o')
ax.plot(times["cores"],times["point"],lw=2,label="Com. point a point",marker='o')
ax.plot(times["cores"],times["global"],lw=2,label="Com. globales",marker='o')

ax.set_title("Fig. 4.1 - temps pour l'étude de weak scaling")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")



#ax.set_yscale("log")

ax.legend(loc="best",ncol=2)

fig0.tight_layout()

fig0.savefig("../../../support/materiel/life_mpi_weak_scaling_time.png")

# ______________________________________________________________________________
# Figure efficacite

fig1 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["efficiency"],lw=2,label="Simulation",marker='o')

ax.plot([times["cores"].min(), times["cores"].max()],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_title("Fig. 4.2 - efficacité pour l'étude de weak scaling")

ax.set_ylim([0.,1.1])

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacité")

ax.legend(loc="best")

fig1.tight_layout()

fig1.savefig("../../../support/materiel/life_mpi_weak_scaling_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["point"]/times["calcul"]*100.,lw=2,color='C2',label='Echange particules',marker='o')
ax.plot(times["cores"],times["global"]/times["calcul"]*100.,lw=2,color='C3',label='Communications globales',marker='o')

ax.set_title("Fig. 4.3 - part relative des coms pour l'étude de weak scaling")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

fig2.savefig("../../../support/materiel/life_mpi_weak_scaling_part.png")

# _____________________________________________________________________________

show()
