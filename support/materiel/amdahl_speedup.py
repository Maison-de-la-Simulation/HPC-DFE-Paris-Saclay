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

# ______________________________________________________________________________

fig = figure(figsize=(10, 8))
gs = GridSpec(2, 2)
ax = subplot(gs[:,:])

# ______________________________________________________________________________

sequentiel_fractions = np.array([0.,0.01,0.1])

processing_units = np.linspace(1,100,1024)

number_of_units = len(processing_units)

for fraction in sequentiel_fractions:
    speedups = np.zeros(number_of_units)
    speedups = 1./(fraction + (1. - fraction) / processing_units)

    ax.plot(processing_units,speedups)

ax.set_xlabel("Nombre d'unites de calcul")
ax.set_xlabel("Acceleration")

fig.tight_layout()

show()
