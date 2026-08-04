<# ______________________________________________________________________________
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
times['cores'] = np.array([ ... ])
# Temps dans la boucle en temps
times['total'] = np.array([ ... ])

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

ax.set_title("Fig. 4.1 - temps pour l'étude de weak scaling")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Temps (s)")
#ax.set_yscale("log")

ax.legend(loc="best",ncol=2)

fig0.tight_layout()

#fig0.savefig("../../../support/materiel/mandelbulb_mpi_weak_scaling_time.png")

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

#fig1.savefig("../../../support/materiel/mandelbulb_mpi_weak_scaling_efficiency.png")

# _____________________________________________________________________________

show()
