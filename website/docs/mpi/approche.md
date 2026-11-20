# 1. Description de l'approche

## Le paradigme par échange de messages

Dans un modèle par échange de messages, les différents exécutants
(**processus**) ont chacun leurs **données propres**, dans leur mémoire
privée. Ils se les communiquent par **messages**. Ces messages servent à
échanger une ou plusieurs données, ou à se synchroniser.

--8<-- "mpi/message-passing.svg"

Un processus ne peut pas lire directement la mémoire d'un autre : toute
donnée partagée doit être explicitement envoyée et reçue.

## Exécuter un programme MPI

### Langages compilés et Python

En C, C++ ou Fortran, un programme MPI est **compilé** et lié à la
bibliothèque MPI, puis l'exécutable est lancé en parallèle par le
*runtime* MPI. Python ne nécessite pas de compilation : c'est
l'interpréteur qui exécute le script, et le paquet **mpi4py** donne accès
aux fonctions MPI.

Lancé avec `python`, un script crée **un seul processus**. Lancé avec
`mpirun`, le **même script** est exécuté par **plusieurs processus** en
parallèle :

--8<-- "mpi/mpirun.svg"

### La commande `mpirun`

Pour exécuter votre code avec `N` processus :

```bash
mpirun -np N python mon_script.py
```

- `-np` donne le nombre de processus MPI à lancer.
- Si ce nombre est inférieur ou égal au nombre d'unités de calcul
  disponibles, chaque processus s'exécute sur une unité indépendante.
- On peut lancer plus de processus que d'unités de calcul : les
  ressources sont alors partagées. Avec Open MPI, il faut ajouter
  l'option `--oversubscribe` pour l'autoriser.

## Communicateur et rang

Communicateur
:   Ensemble de processus MPI capables de communiquer entre eux. C'est en
    quelque sorte l'annuaire des processus en jeu.

Rang
:   Au sein d'un communicateur, chaque processus est identifié par un
    entier unique, son **rang** (*rank*) : c'est en quelque sorte son
    adresse. Les rangs vont de 0 au nombre de processus moins un.

Le communicateur par défaut, qui regroupe tous les processus, se nomme
`MPI_COMM_WORLD`. Avec `mpirun -np 4 python script.py`, il contient les
rangs 0 à 3. Nous verrons qu'on peut aussi créer des
**sous-communicateurs**.

--8<-- "mpi/communicator.svg"

## Structure d'un programme MPI

Voici, étape par étape, le squelette de tout programme MPI en Python :

```python title="script.py"
import mpi4py

# 1. On rend l'initialisation et la finalisation explicites
mpi4py.rc.initialize = False
mpi4py.rc.finalize = False

from mpi4py import MPI

# 2. On initialise MPI
MPI.Init()

# 3. On récupère le communicateur global
comm = MPI.COMM_WORLD

# 4. On récupère le nombre de processus
number_of_ranks = comm.Get_size()

# 5. Chaque processus récupère son rang dans le communicateur
rank = comm.Get_rank()

print(f"Je suis le rang {rank} sur {number_of_ranks}")

# 6. On finalise MPI
MPI.Finalize()
```

1. **Importer mpi4py**, comme pour toute bibliothèque Python.
2. **Initialiser MPI** : MPI est une bibliothèque qui fonctionne par appel
   à des fonctions. En Python, l'initialisation peut être implicite (elle
   a lieu à l'import), mais nous la rendons explicite dans ce cours.
3. **Récupérer le communicateur global** `MPI.COMM_WORLD`.
4. **Récupérer le nombre de rangs** avec `Get_size()` (`MPI_COMM_SIZE`
   en C et Fortran). Pour `MPI_COMM_WORLD`, c'est le nombre total de
   processus demandé à l'exécution.
5. **Récupérer son rang** avec `Get_rank()` (`MPI_COMM_RANK`).
6. **Finaliser MPI** proprement à la fin du programme. C'est surtout très
   important en C, C++ et Fortran.

!!! example "Exercice 1 — Votre premier programme MPI"

    Concevoir vos premiers programmes MPI et les lancer.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/1_initialization)

    Vous pouvez aussi ouvrir l'énoncé dans votre éditeur :
    `exercices/mpi/python/1_initialization/README.md`.

## Notion de parallélisme

On parle de **parallélisme** lorsque chaque processus travaille sur ses
tâches **en même temps** que les autres. La base du parallélisme consiste
donc à découper une charge de travail en sous-tâches et à les distribuer
sur l'ensemble des travailleurs.

--8<-- "mpi/seq-vs-par.svg"

Le parallélisme est **parfait** lorsque tous les travailleurs ont la même
charge de travail. Dans le cas contraire, on parle de **déséquilibre de
charge** : les processus qui ont fini attendent les autres, et ce temps
d'attente est perdu.

--8<-- "mpi/balance.svg"

## Mesurer le temps

`MPI.Wtime()` renvoie le temps écoulé, en secondes, sur le processus
courant. Deux appels et une soustraction donnent le temps passé dans une
section du code :

```python
start = MPI.Wtime()

# Phase de calcul
...

elapsed = MPI.Wtime() - start
```

Comparer ce temps d'un processus à l'autre permet d'estimer le
déséquilibre de charge.

## Différencier le code selon le rang

Le programme s'exécute simultanément autant de fois qu'il y a de
processus : **chaque ligne est exécutée par chaque processus**. Pour
réserver une portion de code à certains processus, on utilise une
condition sur le rang :

```python
rank = comm.Get_rank()

if rank == 1:
    # Actions exécutées uniquement par le processus de rang 1
    ...
```

!!! example "Exercice 2 — Parallélisme parfait"

    Vous familiariser avec la notion de parallélisme en partageant le
    travail entre les processus.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/2_embarrassingly_parallel)

    Dans votre éditeur :
    `exercices/mpi/python/2_embarrassingly_parallel/README.md`.

## Bilan

!!! success "À ce stade, vous savez"

    - écrire un programme parallèle simple ;
    - exécuter un programme MPI avec `mpirun` ;
    - récupérer le nombre de rangs et le rang de chaque processus.
