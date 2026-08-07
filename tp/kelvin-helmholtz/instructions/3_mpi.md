# III. Parallelization of the code via MPI

In this third part, we will parallelize the sequential program using the message-passing paradigm, specifically the MPI standard.

**Preparation :** Make a copy of the `sequential` folder and call it `mpi`. You should work in the `mpi` folder and keep the `sequential` one.

You will parallelize the code step by step. First, it is recommanded to comment the lines of code that are not yet parallelized and to uncomment it step by step as you progress. By this way, you can test the code without crashing.

**Question 3.1 - MPI initialization** 

a) Add in the library section the import of `mpi4py`.

b) After the section `Internal parameters`, we will create a new section called `MPI initialization`. In this section:

- initialiaze MPI
- get the total number of cores available
- get the rank for each MPI process

c) Add the MPI finalization at the end

d) Uncomment the `Parameters summary` section and add the code so that only rank 0 can print the messages. Add additional prints for the MPI properties

e) Run the code to test that the MPI init is working correctly

**Question 3.2 - Domain decomposition**

The next step is the domain decomposition. For simplicity, we will devide the domain into blocks only the `x` direction.

![domain decomposition](../assets/domain_decomposition.png)

For this aim, each rank will have a local version of `omega`, `psi`, `ux` and `uy`.

For `omega` and `psi`, we will have to add extra column for the management of the ghost cells.

a) Add in the global parameters a variable for the number of ranks to use to divide the domain. Add it as well in the `command line arugment` section (`argparse`) so that you can change this parameter by command line.

b) Uncomment the section `domain definition`.

c) Compute for each rank, the local size `nx_local`, the first `ix_start` and last index `ix_end` of the local grid. Then create the local x axis with the extra ghost cells.

d) Update the definition of the meshgrid `X` and `Y` and the different arrays `omega`, `phi`, `ux` and `uy` so that it only represents the local block and not the whole domain.

e) Add in the section `Parameters summary` the print of the local size and indexes, check that the domain is well divided.




