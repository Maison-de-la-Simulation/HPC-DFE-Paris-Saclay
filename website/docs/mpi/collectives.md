# 4. Communications collectives

## Les différents types

Les communications **collectives** font intervenir plusieurs processus,
voire tout le communicateur, pour propager ou rassembler de
l'information. Il en existe trois types :

- la **synchronisation** : c'est la barrière `Barrier` ;
- le **transfert de données** : diffusion, collecte ;
- le **transfert avec opération** sur les données : les **réductions**.

Tous les processus du communicateur doivent appeler la fonction
collective. Les versions standard induisent une barrière implicite pour
les processus concernés : pour chacun, elle est relâchée dès que sa
participation est terminée.

## Diffusion générale : `bcast`

Envoi d'une donnée depuis un processus vers **tous** les processus du
communicateur.

--8<-- "mpi/bcast.svg"

```python
data = comm.bcast(data, root=0)
```

## Diffusion sélective : `scatter`

Partage de données, choisi par le développeur, depuis un processus vers
tous les processus du communicateur : chacun reçoit **sa part**.

--8<-- "mpi/scatter.svg"

```python
# Sur le rang 0, data est une liste d'autant d'éléments que de rangs
part = comm.scatter(data, root=0)
```

## Collecte : `gather`

Envoi de données réparties sur plusieurs processus vers **un processus
unique**, qui les rassemble dans l'ordre des rangs.

--8<-- "mpi/gather.svg"

`gather` est appelée par tous les processus en même temps :

=== "Objets Python"

    ```python
    # Sur le rang 0 : liste des valeurs de tous les rangs ; ailleurs : None
    values = comm.gather(local_value, root=0)
    ```

=== "Tableaux NumPy"

    ```python
    comm.Gather(send_buffer, recv_buffer, root=0)
    ```

    | Paramètre | Rôle |
    |---|---|
    | `send_buffer` | Valeur ou ensemble de valeurs envoyé par chaque processus |
    | `recv_buffer` | Tableau réunissant les valeurs reçues (utilisé sur `root` seulement) |
    | `root` | Processus qui reçoit les données et reconstruit le tableau final |

!!! example "Exercice 7 — Utilisation de `gather`"

    Mettre en pratique la communication collective `gather`.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/7_gather_com)

    Dans votre éditeur : `exercices/mpi/python/7_gather_com/README.md`.

## Réduction : `reduce`

Envoi de données réparties sur plusieurs processus vers **un seul
processus**, avec une **opération de réduction** (somme, produit,
maximum…) réalisée au passage.

--8<-- "mpi/reduce.svg"

`reduce` est appelée par tous les processus en même temps :

```python
reduced = comm.reduce(local_value, op=MPI.SUM, root=0)
```

| Paramètre | Rôle |
|---|---|
| `local_value` | Valeur envoyée par chaque processus |
| `reduced` | Résultat de la réduction, sur `root` seulement (`None` ailleurs) |
| `op` | Opération de réduction |
| `root` | Processus qui reçoit le résultat |

:material-book-open-variant: [Documentation de `Comm.reduce`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.reduce)

### Opérations de réduction

| Opération | Effet |
|---|---|
| `MPI.SUM` | Somme des données |
| `MPI.PROD` | Produit des données |
| `MPI.MAX` | Maximum des valeurs |
| `MPI.MIN` | Minimum des valeurs |

D'autres opérations existent (opérations logiques, position du
maximum…).

### Exemples

=== "Somme sur le rang 0"

    ```python
    local_value = 125.87 * rank
    total = comm.reduce(local_value, op=MPI.SUM, root=0)
    ```

=== "Produit sur le rang 3"

    ```python
    local_value = 5 * rank
    prod = comm.reduce(local_value, op=MPI.PROD, root=3)
    ```

=== "Maximum d'un tableau"

    ```python
    import numpy as np

    # Tableau stocké sur chaque processus
    local_array = np.random.rand(5)

    # Pour chaque indice, maximum sur l'ensemble des processus
    max_array = comm.reduce(local_array, op=MPI.MAX, root=0)

    # Puis maximum sur tous les indices
    if rank == 0:
        global_max = np.max(max_array)
    ```

## Réduction pour tous : `allreduce`

Comme `reduce`, mais le résultat est envoyé à **tous** les processus.

--8<-- "mpi/allreduce.svg"

```python
total = comm.allreduce(local_value, op=MPI.SUM)
```

Il n'y a pas de `root` : chaque processus reçoit le résultat de la
réduction.

:material-book-open-variant: [Documentation de `Comm.allreduce`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.allreduce)

!!! example "Exercice 8 — Utilisation de `reduce`"

    Calculer une intégrale en parallèle à l'aide d'une réduction.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/8_reduce_com)

    Dans votre éditeur : `exercices/mpi/python/8_reduce_com/README.md`.

## Données de taille variable : les variantes `v`

Certaines communications collectives (hors réductions) imposent que
chaque rang échange la **même quantité** de données. Cette limitation est
levée par des variantes, dont le nom se termine par un **v** :
`Gatherv`, `Scatterv`, `Allgatherv`, `Alltoallv`.

### Collecte de tailles variables : `Gatherv`

`Gatherv` envoie des données de **tailles différentes**, réparties sur
plusieurs processus, vers un processus unique. Contrairement à `Gather`,
qui range les contributions dans l'ordre des rangs, `Gatherv` permet
aussi de **choisir où placer** chaque contribution.

```python
comm.Gatherv(send_buffer, [recv_buffer, count, displacement, MPI.DOUBLE], root=0)
```

| Paramètre | Rôle |
|---|---|
| `send_buffer` | Tableau envoyé par chaque processus |
| `recv_buffer` | Tableau réunissant toutes les contributions (sur `root`) |
| `count` | Liste du nombre de valeurs envoyées par chaque rang |
| `displacement` | Position, dans `recv_buffer`, de la première valeur venant de chaque rang |
| `MPI.DOUBLE` | Type des données reçues |
| `root` | Processus qui reçoit les données |

Les listes `count` et `displacement` ont pour taille le nombre de rangs
du communicateur. Avec les mêmes contributions, trois choix de
`displacement` donnent trois rangements différents :

--8<-- "mpi/gatherv.svg"

## Fonctions supplémentaires

Toutes les communications collectives présentées ont aussi un équivalent
**non bloquant** : `Igather`, `Igatherv`, `Iscatter`, `Ibcast`,
`Ialltoall`, `Ireduce`…

D'autres variantes sont à découvrir dans le
[cours de l'IDRIS](http://www.idris.fr/formations/mpi/) et la
documentation de MPI.

## Bilan

!!! success "À ce stade, vous savez"

    - effectuer des communications collectives ;
    - effectuer des réductions.
