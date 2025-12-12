# Exercice : Mise en place d'une topologie cartésienne 2D

Dans cet exercice, nous allons mettre en place une topologie cartésienne 2D.
Au début du programme, les variables suivantes sont définies :

* `ndims` : nombre de dimensions de la topologie
* `dims` : tableau de taille `ndims` contenant le nombre de rangs MPI dans chaque dimension
* `periods` : tableau de taille `ndims` contenant les conditions aux limites de la topologie

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. Complétez la ligne MPI destinées à la création de la topologie cartésienne 2D en utilisant les variables `ndims`, `dims`, et `periods`.

2. Complétez la ligne MPI destinée à récupérer le rang MPI courant dans la topologie cartésienne 2D.

3. Complétez la ligne MPI destinée à récupérer les coordonnées du rang MPI courant dans la topologie cartésienne 2D.

4. Complétez les lignes permettant de récupérer pour chaque rand le rang du voisin à gauche, droite en haut et en bas

5. Fabriquez une matrice 2D pour contenir les valeurs de chaque rang MPI en fonction de leurs coordonnées dans la topologie cartésienne 2D et affichez cette matrice sur le rang 0.

6. Exécutez le code en choisissant un nombre cohérent de processus MPI  par rapport aux dimensions de la topologie cartésienne 2D :

```bash
mpirun -np N python main.py
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