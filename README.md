# Cours de Calcul Haute Performance - Master 2 Dynamique des Fluide et Energétique - Paris Saclay

![CI](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/main.yml/badge.svg)

Ce cours de HPC s'articule autour de 3 dossiers :
- `support`: Les supports de cours (odt et pdf) sont dans cet espace
- `exercices`: Les exercices mentionnés dans le cours sont dans ce dossier.
- `tp`: Ce dossier contient les TPs.
- `documentation` : de l'aide à l'installation des bibliothèques et autre

## Supports de cours

Ce cours vous présente les bases du calcul haute performance et vous donne les
bases de la programmation parallèle par passage de message via le standard MPI.

Le cours se divise en plusieurs parties :
- [Cours d'introduction au calcul haute-performance](./support/Introduction_HPC.pdf)
- [Cours sur MPI](./support/MPI.pdf)
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


- Exercices MPI
  - [Initialisation de MPI](./exercices/mpi/1_initialization).
  - [Initiation aux communications bloquantes](./exercices/mpi/2_blocking_com).
  - [Utilisation de MPI_SENDRECV](./exercices/mpi/3_sendrecv).
  - [Utilisation des communications non bloquantes MPI_ISEND/MPI_IRECV](./exercices/mpi/4_nonblockling_com).
  - [Utilisation de MPI_REDUCE](./exercices/mpi/5_reduce_com).
  - [Utilisation de MPI_GATHER](./exercices/mpi/6_gather_com).
  - [Création d'un communicateur cartésien](./exercices/mpi/7_cartesian_com).
  - [Utilisation du type vector](./exercices/mpi/8_type_vector).
  - [Exercice d'échange de cellules fantômes](./exercices/mpi/9_gather_matrix).

## Travaux pratiques

- [Equation de diffusion (différences finies)](./tp/diffusion/).
- [Equation d'onde (différences finies)](./tp/onde/).
- [Simulation de particules avec collisions](./tp/marble/).
- [Simulation de vie](./tp/life/).
- [Discrétisation des équations de Maxwell par un schéma de Yee](./tp/maxwell/).

## Installation des dépendances

- [MPI](./documentation/mpi.md).
- [HDF5](./documentation/hdf5.md).
- [readme2tex](./documentation/readme2tex.md).
- [Utilisation de Ruche](./documentation/ruche.md).

## Deepnote

Vous pouvez essayer ce projet en ligne grâce au service [Deepnote](./documentation/deepnote.md).
