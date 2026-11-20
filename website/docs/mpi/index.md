# III. Parallélisme par échange de messages avec MPI

Ce chapitre présente **MPI** (*Message Passing Interface*), le standard de
programmation parallèle par échange de messages. MPI permet de répartir
un calcul sur plusieurs processus, sur un même ordinateur ou sur des
milliers de nœuds d'un super-calculateur.

!!! info "Pourquoi Python ?"

    Les codes de simulation sont rarement écrits en Python + MPI : ce sont
    le plus souvent des codes en C, C++ ou Fortran. Pour faciliter
    l'apprentissage, ce cours présente MPI au travers de Python et du
    paquet **mpi4py**. L'API C ou Fortran est un peu plus complexe à
    utiliser, mais les bases sont les mêmes.

## Objectifs

À la fin de ce chapitre, vous saurez :

- écrire et exécuter un programme MPI simple ;
- faire communiquer des processus avec des communications point à point,
  bloquantes ou non bloquantes ;
- utiliser les communications collectives et les réductions ;
- décomposer un domaine de calcul avec une topologie cartésienne.

## Plan du chapitre

<div class="grid cards" markdown>

-   __1. Description de l'approche__

    ---

    Échange de messages, `mpirun`, communicateur et rang, parallélisme et
    équilibrage de charge. **Exercices 1 et 2.**

    [:octicons-arrow-right-24: Lire](approche.md)

-   __2. Communications point à point bloquantes__

    ---

    `send`, `recv`, `sendrecv`, barrières explicites et implicites.
    **Exercices 3 et 4.**

    [:octicons-arrow-right-24: Lire](point-a-point.md)

-   __3. Communications non bloquantes__

    ---

    `isend`, `irecv`, `wait`, recouvrement des communications par le
    calcul. **Exercices 5 et 6.**

    [:octicons-arrow-right-24: Lire](non-bloquantes.md)

-   __4. Communications collectives__

    ---

    `bcast`, `scatter`, `gather`, `reduce`, `allreduce` et leurs variantes.
    **Exercices 7 et 8.**

    [:octicons-arrow-right-24: Lire](collectives.md)

-   __5. Topologie cartésienne__

    ---

    Décomposition de domaine, coordonnées et voisins d'un rang.
    **Exercice 9.**

    [:octicons-arrow-right-24: Lire](topologie.md)

</div>

## Les exercices

Les exercices se trouvent dans le dossier
[`exercices/mpi/python`](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python)
du dépôt. Chacun contient un énoncé (`README.md`), un code à compléter
et un dossier `correction`.

| N° | Exercice | Notion |
|---|---|---|
| 1 | [Premier programme MPI](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/1_initialization) | Initialisation, rang, nombre de rangs |
| 2 | [Parallélisme parfait](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/2_embarrassingly_parallel) | Partage du travail |
| 3 | [Communications bloquantes](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/3_blocking_com) | `send` / `recv` |
| 4 | [Communication en anneau](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/4_sendrecv) | `sendrecv` |
| 5 | [Communications non bloquantes](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/5_nonblocking_com) | `isend` / `irecv` |
| 6 | [Communications de tableaux](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/6_array_com) | Tableaux NumPy |
| 7 | [Collecte](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/7_gather_com) | `gather` |
| 8 | [Réduction](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/8_reduce_com) | `reduce` |
| 9 | [Topologie cartésienne](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/9_cartesian_topology) | `Create_cart`, `Shift` |

## Pour aller plus loin

- [Cours MPI de l'IDRIS](http://www.idris.fr/formations/mpi/) : selon
  moi le cours le plus complet sur MPI, en français et en anglais.
- Les implémentations de MPI fournissent une documentation en ligne
  complète : [Open MPI](https://www.open-mpi.org/),
  [MPICH (Argonne)](https://www.mcs.anl.gov/research/projects/mpi/learning.html).
- [Tutoriel mpi4py](https://mpi4py.readthedocs.io/en/stable/tutorial.html)
  pour MPI en Python.
