# ______________________________________________________________________________
#
# Plot the Amdahl speed-up / efficieny
#
# ______________________________________________________________________________

import numpy as np
from matplotlib import *
from matplotlib.pyplot import *

# ______________________________________________________________________________
# RCParams - personalize the figure output

rcParams['font.family']="Clear Sans Light"

rcParams['figure.facecolor'] = 'w'
rcParams['font.size'] = 20
rcParams['xtick.labelsize'] = 20
rcParams['ytick.labelsize'] = 20
rcParams['axes.labelsize'] = 20

rcParams['xtick.major.size'] = 10
rcParams['ytick.major.size'] = 10

rcParams['xtick.minor.size'] = 5
rcParams['ytick.minor.size'] = 5

rcParams['axes.linewidth'] = 1.5

rcParams['xtick.major.width'] = 2
rcParams['ytick.major.width'] = 2

rcParams['xtick.minor.width'] = 1.5
rcParams['ytick.minor.width'] = 1.5

rcParams['axes.facecolor'] = '#F2F2F2'
#mpl.rcParams['axes.facecolor'] = '#EFEDE8'
rcParams['axes.facecolor'] = 'white'
rcParams['axes.edgecolor'] = 'black'
rcParams['axes.grid'] = 'True'
rcParams['grid.color'] = '#F2F2F2'
rcParams['grid.linestyle'] = '-'
rcParams['grid.linewidth'] = 3
rcParams['axes.axisbelow'] = 'True'

# ______________________________________________________________________________

fig = figure(figsize=(12, 6))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

# ______________________________________________________________________________

sequentiel_fractions = np.array([0.,0.01,0.1])

processing_units = np.linspace(1,100,1024)

number_of_units = len(processing_units)

for fraction in sequentiel_fractions:
    efficiency = np.zeros(number_of_units)
    efficiency = 1./(fraction + (1. - fraction) / processing_units) / processing_units

    ax.plot(processing_units,efficiency,lw=2,label=r"$\alpha_s$ = {} ".format(fraction))

ax.set_xlabel("Nombre d'unites de calcul")
ax.set_ylabel("Efficacite")

ax.set_ylim([0,1.2])
ax.set_xlim([1,100])

ax.legend(loc="upper center",ncol=3)

fig.tight_layout()

fig.savefig("Amdahl_efficiency.png",dpi=100,transparent=True)

show()
