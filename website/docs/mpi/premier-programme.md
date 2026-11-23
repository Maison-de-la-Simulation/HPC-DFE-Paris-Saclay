# 2. Premier programme parallèle

Ce chapitre montre comment écrire un premier programme réellement
parallèle : découper le travail entre les processus, puis mesurer ce que
la parallélisation apporte.

## Notion de parallélisme

On parle de **parallélisme** lorsque chaque processus travaille sur ses
tâches **en même temps** que les autres. La base du parallélisme consiste
donc à découper une charge de travail en sous-tâches et à les distribuer
sur l'ensemble des travailleurs.

Dans ce chapitre, nous utilisons le **parallélisme de données** (voir
[Notion de programme parallèle](../introduction/programme-parallele.md)) :
tous les processus exécutent **le même calcul**, chacun sur **sa part des
données**.

--8<-- "mpi/seq-vs-par.svg"

Le parallélisme est **parfait** lorsque tous les travailleurs ont la même
charge de travail. Dans le cas contraire, on parle de **déséquilibre de
charge** : les processus qui ont fini attendent les autres, et ce temps
d'attente est perdu.

--8<-- "mpi/balance.svg"

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

## Répartir le travail entre les rangs

Pour faire du parallélisme de données, chaque rang utilise son numéro pour
déterminer **sa part** du travail. Prenons une boucle de `N` itérations à
répartir entre les rangs : on découpe l'intervalle `[0, N)` en blocs
contigus, un par rang.

```python
number_of_ranks = comm.Get_size()

N = 10                      # nombre total d'itérations

# Nombre d'itérations par rang (les premiers rangs prennent le reste)
n_local = N // number_of_ranks
reste = N % number_of_ranks
if rank < reste:
    n_local += 1

# Indices de début et de fin du bloc de ce rang
start = rank * (N // number_of_ranks) + min(rank, reste)
end = start + n_local

local_sum = 0
for i in range(start, end):
    local_sum += i

print(f"Rang {rank} : itérations {start} à {end - 1}, somme partielle = {local_sum}")
```

Avec 4 processus, les 10 itérations se répartissent ainsi :

| Rang | Itérations | Somme partielle |
|---|---|---|
| 0 | 0 à 2 | 3 |
| 1 | 3 à 5 | 12 |
| 2 | 6 à 7 | 13 |
| 3 | 8 à 9 | 17 |

Comme 10 n'est pas divisible par 4, les deux premiers rangs reçoivent une
itération de plus. La somme totale (45) s'obtient en additionnant les
sommes partielles : c'est le rôle d'une **réduction**, que nous verrons
dans la partie sur les [réductions](reductions.md).

## Mesurer le temps d'exécution

Mesurer le temps passé dans les différentes parties d'un programme permet
de savoir ce que la parallélisation apporte, et de repérer les
**déséquilibres de charge** entre processus.

### La fonction `MPI.Wtime`

`MPI.Wtime()` renvoie le temps écoulé, en secondes, sur le processus
courant, sous forme d'un nombre réel. Sa valeur absolue n'a pas de sens :
seule la **différence** entre deux appels est utile. Deux appels et une
soustraction donnent donc le temps passé dans une section du code :

=== "Python"

    ```python
    start = MPI.Wtime()

    # Phase de calcul
    ...

    elapsed = MPI.Wtime() - start
    ```

=== "C / C++"

    ```cpp
    double start = MPI_Wtime();

    // Phase de calcul
    // ...

    double elapsed = MPI_Wtime() - start;
    ```

:material-book-open-variant: Documentation :
[`MPI.Wtime` (mpi4py)](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Wtime.html) ·
[`MPI_Wtime` (RookieHPC)](https://rookiehpc.github.io/mpi/docs/mpi_wtime/index.html)

La fonction `MPI.Wtick()` donne la **résolution** de l'horloge, c'est-à-dire
le plus petit écart de temps mesurable (de l'ordre de la microseconde sur
une machine courante). Une section plus courte que cette résolution ne
peut pas être mesurée de façon fiable.

### Chaque processus mesure son propre temps

Chaque processus a sa propre horloge et mesure **son** temps. Comparer ces
temps d'un processus à l'autre permet d'estimer le déséquilibre de charge.

Dans l'exemple suivant, chaque rang « travaille » d'autant plus longtemps
que son numéro est élevé :

```python
import time

start = MPI.Wtime()
time.sleep(0.1 * (rank + 1))        # simule un calcul de durée variable
elapsed = MPI.Wtime() - start

print(f"Rang {rank} : {elapsed:.3f} s")
```

Avec 4 processus, on obtient environ `0.1 s`, `0.2 s`, `0.3 s` et `0.4 s` :
le rang 3 travaille 4 fois plus longtemps que le rang 0, qui attendra
ensuite les autres.

!!! info "Pour aller plus loin"

    Pour mesurer le temps total d'une section et rassembler les temps sur
    un rang, il faut des communications. Nous le verrons à la fin de la
    partie sur les
    [communications bloquantes](point-a-point.md#mesurer-le-temps-dune-section-parallele).

!!! example "Exercice 2 — Parallélisme parfait"

    Vous familiariser avec la notion de parallélisme en partageant le
    travail entre les processus.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/embarrassingly_parallel)

    Dans votre éditeur :
    `exercices/mpi/python/embarrassingly_parallel/README.md`.

## Bilan

!!! success "À ce stade, vous savez"

    - répartir le travail entre les processus grâce à leur rang ;
    - réserver une portion de code à certains processus ;
    - mesurer le temps passé dans une section de code avec `MPI.Wtime` ;
    - comparer les temps des processus pour repérer un déséquilibre.
