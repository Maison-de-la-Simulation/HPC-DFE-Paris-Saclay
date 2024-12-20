#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output_weak
#SBATCH --error=error_weak            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=40                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:10:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
mpirun -np 1 python3 main.py -N 320000000
mpirun -np 2 python3 main.py -N 640000000
mpirun -np 4 python3 main.py -N 1280000000
mpirun -np 8 python3 main.py -N 2560000000
mpirun -np 16 python3 main.py -N 5120000000
mpirun -np 20 python3 main.py -N 6400000000
mpirun -np 32 python3 main.py -N 10240000000
mpirun -np 40 python3 main.py -N 12800000000
