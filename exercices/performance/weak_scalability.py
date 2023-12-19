import numpy as np
from matplotlib import *
import matplotlib.pyplot as plt

cores = [1,2,4,8,16,20]

time = [7.943760941911023, 8.326548831901164, 8.44030700881558, 8.441214710474014, 8.632096801724401, 9.607745611046266]
        
efficiency = time[0]/(np.array(time))

# Plot the efficiency using Matplotlib

fig0 = plt.figure(figsize=(12, 6))
gs = plt.GridSpec(2, 2)
ax = plt.subplot(gs[:,:])

ax.plot(cores,efficiency,lw=2,label="Simulation",marker='o')

ax.plot([cores[0], cores[-1]],[1,1],lw=2,label="Scalabilite parfaite")

ax.set_xlabel("Nombre de processus")
ax.set_ylabel("Efficacité")
ax.set_title("Etude de weak scaling")
ax.set_ylim([0.,1.1])

plt.show()
