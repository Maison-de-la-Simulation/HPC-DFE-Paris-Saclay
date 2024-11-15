# Cours de Calcul Haute Performance - Master 2 Dynamique des Fluide et Energétique - Paris Saclay

![CI](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/main.yml/badge.svg)

Ce cours de HPC s'articule autour de 4 dossiers :
- `support`: Les supports de cours (odt et pdf) sont dans cet espace
- `exercices`: Les exercices mentionnés dans le cours sont dans ce dossier.
- `tp`: Ce dossier contient les TPs.
- `documentation` : de l'aide à l'installation des bibliothèques et autre

## Supports de cours

Ce cours vous présente les bases du calcul haute performance et vous donne les
bases de la programmation parallèle par passage de message via le standard MPI.

Le cours se divise en plusieurs parties :
- [Cours d'introduction au calcul haute-performance](./support/Introduction_HPC.pdf)
- [Cours sur MPI en Python](./support/MPI_python.pdf)
- [Cours sur MPI en C++](./support/MPI_python.pdf)
- [Cours sur OpenMP](./support/OpenMP.pdf)
- [Cours sur la mesure de la performance](./support/mesure_performance.pdf)
- [Bonnes pratiques](./support/bonnes_pratiques.pdf)

## Exercices

Il y a plusieurs exercices en lien avec le cours que vous trouverez
dans [le dossier exercices](./exercices/).

- Exercices OpenMP
  - [Région parallèle](./exercices/openmp/1_omp_parallel)
  - [Boucle parallèle](./exercices/openmp/2_omp_do)
  - [Région critical et atomics](./exercices/openmp/3_omp_critical)

- Exercices MPI Python
  - [Initialisation de MPI](./exercices/mpi/python/1_initialization).
  - [Parallélisme parfait](./exercices/mpi/python/2_embarrassingly_parallel).
  - [Initiation aux communications bloquantes](./exercices/mpi/python/3_blocking_com).
  - [Utilisation de MPI_SENDRECV](./exercices/mpi/python/4_sendrecv).
  - [Utilisation des communications non bloquantes MPI_ISEND/MPI_IRECV](./exercices/mpi/python/5_nonblockling_com).
  - [Utilisation de MPI_REDUCE](./exercices/mpi/python/6_array_com).
  - [Utilisation de MPI_GATHER](./exercices/mpi/python/7_gather_com).
  - [Réduction MPI.Reduce](./exercices/mpi/python/8_reduce_com).
  - [Exercice de construction d'une topologie cartésienne](./exercices/mpi/python/9_cartesian_topology).

- Exercices MPI C++
  - [Initialisation de MPI](./exercices/mpi/cpp/1_initialization).
  - [Initiation aux communications bloquantes](./exercices/mpi/cpp/2_blocking_com).
  - [Utilisation de MPI_SENDRECV](./exercices/mpi/cpp/3_sendrecv).
  - [Utilisation des communications non bloquantes MPI_ISEND/MPI_IRECV](./exercices/mpi/cpp/4_nonblockling_com).
  - [Utilisation de MPI_REDUCE](./exercices/mpi/cpp/5_reduce_com).
  - [Utilisation de MPI_GATHER](./exercices/mpi/cpp/6_gather_com).
  - [Création d'un communicateur cartésien](./exercices/mpi/cpp/7_cartesian_com).
  - [Utilisation du type vector](./exercices/mpi/cpp/8_type_vector).
  - [Exercice d'échange de cellules fantômes](./exercices/mpi/cpp/9_gather_matrix).

## Travaux pratiques

- [Equation de diffusion (différences finies)](./tp/diffusion/).
- [Equation d'onde (différences finies)](./tp/onde/).
- [Simulation de particules avec collisions](./tp/marble/).
- [Simulation de vie](./tp/life/).
- [Discrétisation des équations de Maxwell par un schéma de Yee](./tp/maxwell/).
- [Equation en eau peu profonde](./tp/shallow/).
- [Mandelbulb](./tp/mandelbulb/).

## Installation des dépendances

- [MPI](./documentation/mpi.md).
- [HDF5](./documentation/hdf5.md).
- [readme2tex](./documentation/readme2tex.md).
- [Utilisation de Ruche](./documentation/ruche.md).

## Deepnote

Vous pouvez essayer ce projet en ligne grâce au service [Deepnote](./documentation/deepnote.md).
