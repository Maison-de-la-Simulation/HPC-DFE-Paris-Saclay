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
times["cores"]       = np.array([1,2,4,8,16,32,48,64,96,128])
# Temps dans la boucle en temps
times["total"] = np.array([1481.007591,817.275929, 483.945553, 323.134755,240.653431,196.507436,178.393044,169.432511,162.187075, 157.878737 ])
# Temps dans la boucle en temps
times["calcul"] = np.array([1342.429612,678.976874, 347.362091, 186.846782,104.280626,60.183146,41.867788,32.976004,26.679670,22.462243])
# Temps pour les collisions
times["point"] = np.array([3.521534,70.906168,102.840178, 102.347629,94.118631,110.748023,113.655305,115.124483,117.117469,116.335050])
# Temps dans le pousseur
times["global"] = np.array([0.004365,0.011119, 0.011932,16.900119,33.669451,21.153652,19.821558,19.113659,16.903380,17.950769])


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

ax0.plot(times["cores"],times["total"],lw=2,label="Total",marker='o')
ax0.plot(times["cores"],times["calcul"],lw=2,label="Calcul",marker='o')
ax0.plot(times["cores"],times["point"],lw=2,label="Com. point a point",marker='o')
ax0.plot(times["cores"],times["global"],lw=2,label="Com. globales",marker='o')

ax0.set_xlabel("Nombre de processus")
ax0.set_ylabel("Temps (s)")

# ax1.set_xlabel("Nombre de processus")
# ax1.set_ylabel("Temps (s)")

ax0.set_title("Fig. 4.4 - Strong scaling MPI")

# ax0.set_yscale("log")

ax0.legend(loc="best",ncol=2)


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

ax.plot(times["cores"],times["calcul"]/times["total"]*100.,lw=2,marker='o',color='C1',label='Calcul')
ax.plot(times["cores"],times["point"]/times["total"]*100.,lw=2,marker='o',color='C2',label='Com point a point')
ax.plot(times["cores"],times["global"]/times["total"]*100.,lw=2,marker='o',color='C3',label='Communications globales')

ax.set_title("Fig. 4.6 - Strong scaling MPI")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Part (%)")

ax.legend(loc="best")

fig2.tight_layout()

fig2.savefig("../../../support/materiel/life_mpi_strong_scaling_part.png")

# _____________________________________________________________________________

show()
