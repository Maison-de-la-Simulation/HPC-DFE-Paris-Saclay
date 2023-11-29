#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --error=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --nodes=1                 # Nombre de noeuds à exploiter
#SBATCH --time=00:10:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# on charge les modules en un coup
source ~/env_dfe_hpc.sh

# On regarde les propriétés des processeurs
srun ./exe