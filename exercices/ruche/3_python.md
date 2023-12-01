# Exercice: lancement d'un script Python sur un noeud de calcul

Cet exercice a pour but de vous familiariser avec le lancement de script Python sur un noeud de calcul.


1) Nous allons maintenant préparer un second script pour lancer un script Python depuis un noeud de calcul.

Vous allez d'abord créer un nouveau dossier `test_slurm_python` et vous y rendre :

```bash
mkdir test_slurm_python
cd test_slurm_python
```

2) Concevez un script Python qui affiche Hello World dans un fichier `hello.py` et attend 10 secondes avant de se terminer.


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

python hello.py
```

4) Lancez le job en faisant :

```bash
sbatch launch.slurm
```

5) Vérifiez que le job est bien en cours d'exécution en faisant :

```bash
squeue -u <login>
```

11) Une fois le job terminé, vous devriez voir deux fichiers `output` et `error` qui ont été créés. Regardez leur contenu en faisant :

```bash
cat output
cat error
```

Pour le moment nous ne lançons que des jobs séquentiel, c'est à dire en n'utilisant qu'une seule unité de calcul.
Dans la partie du cours sur MPI, nous verrons comment lancer des jobs parallèles.