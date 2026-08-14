# IV. Etude de performance

In this final part, we will now examine the performance of the parallel code. To do so, we will look at the two types of scalability: weak and strong scaling. In each case, you will conduct your own study using the Ruche supercomputer.

### 1. Weak scaling study

Weak scaling involves increasing the problem size proportionally to the number of cores. The workload per computing unit, and therefore per core, remains constant. Weak scaling evaluates a code's ability to simulate a larger problem with increased resources within a given timeframe.

On utilisera les paramètres suivants :

- domain discretization: `256 x 128` per core
- final time: 20.0

a) Conduct the study using your parallelized code with the number of cores that you have in your computer.

b) Compute the total quantity of data to exchange using the point-to-point communication as a function of the number of ranks. Do the same for the global communications (reduction).

c) Plot the computation time (y-axis) against the number of cores (x-axis). It is recommended to perform multiple runs and average the execution time to smooth out fluctuations.

d) Plot the efficiency curve (y-axis) against the number of cores (x-axis). Include a reference line representing perfect efficiency.

e) Analyze the plots and discuss the results."

### 2. Strong scaling study

Strong scaling involves keeping the problem size constant while increasing the number of cores. The workload per computing unit decreases proportionally with the increase in the number of cores. Strong scaling evaluates a code's ability to solve the same problem using more resources, for instance, with the aim of achieving a faster execution time.

On utilisera les paramètres suivants :

- domain discretization: `256 x 128 ` par core
- final time: 20.0
 
a) Conduct the study using your parallelized code with the number of cores that you have in your computer.

b) Plot the computation time (y-axis) against the number of cores (x-axis). Indicate the total time on the plot.

c) Plot the efficiency curve (y-axis) against the number of cores (x-axis). Include a reference line representing perfect efficiency.

d) Analyze the plots and discuss the results."