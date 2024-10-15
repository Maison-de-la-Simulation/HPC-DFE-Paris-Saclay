# ______________________________________________________________________________
#
# Passage à l'échelle faible
#
# ______________________________________________________________________________

import numpy as np
from matplotlib import *
from matplotlib.pyplot import *

# ______________________________________________________________________________
# Les donnees

times = {}

# Nombre de processus
times['cores'] = np.array([1, 2, 4, 8, 16, 32, 40, 64, 80])
# Temps dans la boucle en temps
times['total'] = np.array([4.746, 4.961, 4.977, 5.525, 10.346, 9.05, 12.474, 12.541, 12.856])
# Temps dans les communications point e point
times['com'] = np.array([0.012, 0.189, 0.146, 0.2, 0.45, 0.211, 0.21, 0.355, 0.255])
# Temps pour les reductions de l'energie
times['reduc'] = np.array([0.082, 0.085, 0.156, 0.144, 0.197, 0.26, 0.269, 0.907, 0.536])

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
ax.plot(times["cores"],times["com"],lw=2,label="com point a point",marker='o')
ax.plot(times["cores"],times["reduc"],lw=2,label="energy",marker='o')

ax.set_title("Fig. 4.1 - temps pour l'étude de weak scaling")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")



#ax.set_yscale("log")

ax.legend(loc="best",ncol=2)

fig0.tight_layout()

#fig0.savefig("../../../support/materiel/maxwell_mpi_weak_scaling_time.png")

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

#fig1.savefig("../../../support/materiel/maxwell_mpi_weak_scaling_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["com"]/times["total"]*100.,lw=2,color='C1',label='com point a point',marker='o')
ax.plot(times["cores"],times["reduc"]/times["total"]*100.,lw=2,color='C2',label='reduction energy',marker='o')

ax.set_title("Fig. 4.3 - part relative des coms pour l'étude de weak scaling")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

#fig2.savefig("../../../support/materiel/maxwell_mpi_weak_scaling_part.png")

# _____________________________________________________________________________

show()
