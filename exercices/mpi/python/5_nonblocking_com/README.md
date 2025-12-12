# Exercice 5 : communications point à point non bloquantes

Cet exercice met en pratique l'utilisation des communications non-bloquantes `isend` et `irecv`.
Pour cela, l'exercice reprend le principe de l'anneau de communication implémenté  dans l'exercice précédent.

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. Ajoutez les appels à `isend` et `irecv` en prenant soin de placer les bons paramètres.

2. Ajoutez les appels à `wait` pour finaliser les communications et récupérer le message envoyé.

3. Executez le code comme pour l'exercice précédent en variant le nombre de processus `N` :

```bash
mpirun -np N ./executable
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