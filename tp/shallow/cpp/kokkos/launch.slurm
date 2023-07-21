#!/bin/bash
#SBATCH --job-name=train
#SBATCH --output=output
##SBATCH --error=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --nodes=1                 # Nombre de noeuds à exploiter
#SBATCH --time=01:00:00           # Temps souhaité pour la réservation
#SBATCH --gres=gpu:1
#SBATCH --partition=gpu_test

# on charge les modules en un coup
source ~/env/kokkos

srun ./shallow