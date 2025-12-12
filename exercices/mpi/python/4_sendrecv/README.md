# Communication point à point sendrecv

Cet exercice présente le fonctionnement des communications
bloquantes de type `sendrecv`.
On illustre ici son fonctionnement par une communication en anneau.
Chaque processus envoie un message à son voisin de rang supérieur
et reçoit de son voisin de rang inférieur.
On effectue un anneau périodique.

On fait en sorte que chaque rang envoie au rang supérieur la valeur de son propre rang.

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. On va stocker dans la variable `send_rank` à qui on envoie le message et dans `recv_rank` de qui on reçoit.
La première étape consiste à déterminer le contenu de ces variables pour chaque rang pour effectuer l'anneau de communication décrit en introduction.

```python
    if (rank == 0) {
        send_rank = ???;
        recv_rank = ???;
    } else if (rank == number_of_ranks - 1) {
        send_rank = ???;
        recv_rank = ???;
    } else {
        send_rank = ???;
        recv_rank = ???
    }
```

2. Complétez ensuite la fonction `sendrecv` pour effectuer l'anneau de communication décrit en introduction.

3. Exécutez le code sur au moins 2 processus pour que l'exécution soit intéressante. Faites varier ce nombre.

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