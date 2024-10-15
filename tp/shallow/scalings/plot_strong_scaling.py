# ______________________________________________________________________________
#
# Passage à l'échelle fort
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
times['total'] = np.array([103.479, 50.835, 22.289, 10.839, 6.759, 5.483, 5.179, 2.53, 1.884])
# Temps dans les communications point e point
times['com'] = np.array([0.003, 0.281, 0.193, 0.214, 0.138, 1.01, 0.064, 0.183, 0.117])
# Temps pour les reductions de l'energie
times['reduc'] = np.array([0.022, 0.115, 0.027, 0.073, 0.138, 0.052, 0.088, 0.429, 0.108])

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
ax0 = subplot(gs[:,:])

ax0.plot(times["cores"],times["total"],lw=2,label="total",marker='o')
ax0.plot(times["cores"],times["com"],lw=2,label="com point a point",marker='o')
ax0.plot(times["cores"],times["reduc"],lw=2,label="reduction energy",marker='o')

ax0.set_xlabel("Nombre de processus")
ax0.set_ylabel("Temps (s)")

# ax1.set_xlabel("Nombre de processus")
# ax1.set_ylabel("Temps (s)")

ax0.set_title("Fig. 4.4 - Strong scaling MPI")

# ax0.set_yscale("log")

ax0.legend(loc="best",ncol=2)


fig0.tight_layout()

#fig0.savefig("../../../support/materiel/shallow_mpi_strong_scaling_time.png")

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

#fig1.savefig("../../../support/materiel/maxwell_mpi_strong_scaling_efficiency.png")

# ______________________________________________________________________________
# Figure Part MPI

fig2 = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

ax.plot(times["cores"],times["com"]/times["total"]*100.,lw=2,marker='o',color='C1',label='com point a point')
ax.plot(times["cores"],times["reduc"]/times["total"]*100.,lw=2,marker='o',color='C2',label='reduction energy')

ax.set_title("Fig. 4.6 - Strong scaling MPI")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

#fig2.savefig("../../../support/materiel/maxwell_mpi_strong_scaling_part.png")

# _____________________________________________________________________________

show()
