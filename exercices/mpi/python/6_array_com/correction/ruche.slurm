#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --error=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=10                # Nombre d'unité de calcul ou de processus MPI
##SBATCH --nodes=1
##SBATCH --exclusive
#SBATCH --time=00:10:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

set -x

source /gpfs/workdir/labotm/Installations/miniconda3/m2dfe_env.sh
mpirun -np 10 python3 main.py


