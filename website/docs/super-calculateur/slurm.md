# 3. SLURM et lancement d'un premier job

## Présentation de SLURM

[SLURM](https://slurm.schedmd.com/quickstart.html) est un gestionnaire de
super-calculateurs et un **ordonnanceur de tâches** (*jobs*). C'est l'un
des plus utilisés aujourd'hui dans le monde. Il fournit à l'utilisateur
des commandes pour soumettre des jobs, les gérer et les analyser.

Job
:   Un ensemble de commandes que l'on souhaite faire exécuter par le
    super-calculateur. Il comprend l'exécution du code, mais pas
    seulement.

Script SLURM
:   Le fichier qui décrit un job. C'est un script *shell* dont l'en-tête
    (lignes `#SBATCH`) indique à SLURM les conditions d'exécution :
    nombre de processus, durée, partition…

:material-book-open-variant: [Gestion des jobs SLURM sur Ruche](https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/06_slurm_jobs_management/)

## Exemple de script SLURM

```bash title="launch.slurm"
#!/bin/bash
#SBATCH --job-name=master_dfe     # Nom du job
#SBATCH --output=output           # Fichier qui reçoit la sortie standard
#SBATCH --error=error             # Fichier qui reçoit la sortie d'erreur
#SBATCH --ntasks=1                # Nombre d'unités de calcul ou de processus MPI
#SBATCH --nodes=1                 # Nombre de nœuds à exploiter
#SBATCH --time=00:05:00           # Durée souhaitée pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# S'assure que l'exécution a lieu dans le dossier de soumission du job
cd $SLURM_SUBMIT_DIR

set -x

# Affiche les propriétés des processeurs
lscpu

# Attend 10 secondes pour faire durer un peu le job
sleep 10
```

Il existe un grand nombre d'autres options SLURM pouvant être passées en
en-tête. Pour aller plus loin :

- [Gestion des jobs sur Ruche](https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/06_slurm_jobs_management/)
- [Exemples de scripts SLURM sur Ruche](https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/08_slurm_examples/)
- [Résumé des commandes SLURM (PDF)](https://slurm.schedmd.com/pdfs/summary.pdf)

## Les commandes SLURM

| Commande | Effet |
|---|---|
| `sbatch launch.slurm` | Soumet le script : le job est placé dans la file d'attente |
| `squeue -a` | Affiche toute la file d'attente |
| `squeue -u <login>` | Affiche l'état de vos jobs |
| `scancel <job id>` | Annule un job en attente ou en cours d'exécution |
| `sinfo` | Affiche l'état des partitions |

Un exemple de session :

```bash
sbatch launch.slurm      # soumission, SLURM renvoie le numéro du job
squeue -u <login>        # suivi de vos jobs
scancel <job id>         # annulation si besoin
```

Vous pouvez soumettre plusieurs scripts à la fois : c'est SLURM qui
décide quand les lancer. Le numéro de job (*job id*) affiché par `squeue`
sert à annuler un job avec `scancel`.

D'autres commandes SLURM existent ; nous ne les utiliserons pas dans ce
cours, mais vous les trouverez dans la documentation de Ruche ou dans
celle de SLURM.

## Exercice 2 — Lancement d'un premier job

!!! example "Exercice 2"

    Cet exercice a pour but de vous apprendre à écrire vos premiers
    scripts SLURM et à les exécuter.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/blob/master/exercices/ruche/2_slurm.md)

    Vous pouvez aussi l'ouvrir directement sur Ruche, ou dans votre
    éditeur :

    ```bash
    vim exercices/ruche/2_slurm.md
    ```

## Testez-vous

??? question "Votre job reste en attente dans `squeue`. Est-ce une erreur ?"

    Non : SLURM attend que les ressources demandées se libèrent et
    applique des règles de priorité entre les utilisateurs. Plus vous
    demandez de ressources ou de temps, plus l'attente peut être longue.

??? question "Où trouver ce que votre programme a affiché ?"

    Dans les fichiers indiqués par `--output` (sortie standard) et
    `--error` (sortie d'erreur), ici `output` et `error`, dans le dossier
    de soumission.
