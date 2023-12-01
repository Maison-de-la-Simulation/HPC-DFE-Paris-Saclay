# Exercice: lancement d'un script Python sur un noeud de calcul en parallel

Cet exercice a pour but de vous familiariser avec le lancement de script Python sur un noeud de calcul.

Nous allons préparer un script pour lancer un script Python depuis un noeud de calcul en utilisant plusieurs coeurs.

N'oubliez pas de charger votre environnement :

```bash
source /gpfs/workdir/labotm/Installations/miniconda3/m2dfe_env.sh
```

1) Vous allez d'abord créer un nouveau dossier `test_slurm_python_parallel` et vous y rendre :

```bash
mkdir test_slurm_python_parallel
cd test_slurm_python_parallel
```

2) Reprenez le script Python de l'exercice 1 sur MPI et copiez le dans le dossier cet exercice.


3) Créez un script `launch.slurm` avec le contenu suivant :

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe     # Nom du job
#SBATCH --output=output           # fichier qui réceptionne la sortie standard
#SBATCH --error=error             # fichier qui réceptionne la sortie d'erreur
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=00:05:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# Permet de s'assurer que l'exécution a lieu dans le dossier de soumission du job
cd $SLURM_SUBMIT_DIR

set -x

mpirun -np 1 python main.py
```

Il s'agit ici d'un script séquentiel car nous n'utilisons qu'un seul coeur.
Le paramètre `--ntasks` indique le nombre de coeurs que nous souhaitons utiliser.
Si le nombre de coeurs est supérieur à 1, alors le script sera exécuté en parallèle.
Si le nombre de coeurs dépasse le nombre de coeurs par noeud, alors le script sera exécuté sur plusieurs noeuds pour satisfaire le nombre total de coeurs demandé.


4) Lancez le job en demandant 2 coeurs et observez le résultat dans le fichier `output`. Vous devez également ajuster le nombre de processus MPI demandé.

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe     # Nom du job
#SBATCH --output=output           # fichier qui réceptionne la sortie standard
#SBATCH --error=error             # fichier qui réceptionne la sortie d'erreur
#SBATCH --ntasks=2                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=00:05:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# Permet de s'assurer que l'exécution a lieu dans le dossier de soumission du job
cd $SLURM_SUBMIT_DIR

set -x

mpirun -np 2 python main.py
```

5) Vous pouvez augmenter le nombre de coeurs pour augmenter le parallélisme. 
