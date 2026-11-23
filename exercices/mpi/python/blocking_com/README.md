# Fonctionnement des communications point à point bloquantes SEND/RECV

Ce deuxième exercise a pour but d'illustrer l'utilisation des communications bloquantes point à point.
Dans ce code, le processus MPI de rang 1 envoie un message au processus
de rang 2.

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. Prenez le temps de regarder le code dans sa globalité.

La récupération des rangs `rank` permet d'effectuer des actions différenciées pour
chaque processus par l'intermédiaire d'une simple condition `if`.
Ici, l'envoi est effectué par le rang 1 et la réception par le rang 2 grâce à la condition `if`.

2. Complétez la fonction qui permet au rang 1 d'envoyer la variable `message` de
 type entier (`integer`) au rang 2.

```python
comm.send(...)
```

3. Complétez maintenant la fonction qui permet de réceptionner le
message venant du rang 1.

```python
message = comm.recv(...)
```


5. Exécuter le code avec au moins 3 processus sur la frontale ou sur un noeud de calcul :

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