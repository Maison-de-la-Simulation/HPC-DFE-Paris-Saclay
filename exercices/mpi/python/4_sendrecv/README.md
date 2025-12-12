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

**Warning**: n'oubliez pas de charger l'environnement Python avant d'exécuter le code.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```