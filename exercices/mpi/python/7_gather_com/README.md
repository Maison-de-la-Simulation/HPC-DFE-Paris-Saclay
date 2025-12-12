# Exercice : Communication collective MPI_GATHER

Dans cet exercice nous allons simplement mettre en pratique la communication collective `gather`.

Ici, chaque processus MPI a un tableau de taille `size` avec pour valeur initiale son rang MPI.
Le but de l'exercice est d'envoyer sur le rang 0 les tableaux de tous les processus MPI pour reconstruire un tableau global.

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. Complétez la taille du tableau global et son allocation sur le processus 0

2. Mettre ensuite en place la communication collective `gather` pour récupérer les tableaux de tous les processus MPI sur le processus 0

3. Exécutez le code en utilisant plusieurs processus

Par exemple avec 4 processus MPI :

```bash
mpirun -np 4 python main.py
```

## Rappel

Vous pouvez utiliser l'exemple de script SLURM suivant pour exécuter votre code sur un noeud de Ruche :

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --error=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=00:10:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh

mpirun -np 1 python3 main.py
```

Pour rappel, on exécute le script avec la commande `sbatch` :

```bash
sbatch launch.slurm
```

Vous pouvez modifier le paramètre `--ntasks` pour exécuter votre code avec plusieurs processus MPI.