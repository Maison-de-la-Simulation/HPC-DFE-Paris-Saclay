# Cours de Calcul Haute Performance - Master 2 Dynamique des Fluide et Energétique - Paris Saclay

[![CI](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/main.yml/badge.svg)](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/main.yml)
[![Site web](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/website.yml/badge.svg?branch=master)](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/actions/workflows/website.yml)

> **Site web du cours : <https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/>**

Ce cours de HPC s'articule autour de 4 dossiers :
- `exercices`: Les exercices mentionnés dans le cours sont dans ce dossier.
- `tp`: Ce dossier contient les TPs.
- `documentation` : de l'aide à l'installation des bibliothèques et autre
- `website` : contient les pages et les assets pour le déploiement du site web

## Exercices

Il y a plusieurs exercices en lien avec le cours que vous trouverez
dans [le dossier exercices](./exercices/).

- Exercices MPI Python
  - [Initialisation de MPI](./exercices/mpi/python/initialization).
  - [Parallélisme parfait](./exercices/mpi/python/embarrassingly_parallel).
  - [Initiation aux communications bloquantes](./exercices/mpi/python/blocking_com).
  - [Utilisation de MPI_SENDRECV](./exercices/mpi/python/sendrecv).
  - [Utilisation des communications non bloquantes MPI_ISEND/MPI_IRECV](./exercices/mpi/python/nonblocking_com).
  - [Utilisation de MPI_REDUCE](./exercices/mpi/python/array_com).
  - [Utilisation de MPI_GATHER](./exercices/mpi/python/gather_com).
  - [Réduction MPI.Reduce](./exercices/mpi/python/reduce_com).
  - [Exercice de construction d'une topologie cartésienne](./exercices/mpi/python/cartesian_topology).

- Exercice HDF5
  - [Écriture parallèle de fichiers HDF5](./exercices/hdf5).

- Exercice FFT distribuée
  - [Équation de Poisson par FFT distribuée](./exercices/fft).

- Exercices MPI C++
  - [Initialisation de MPI](./exercices/mpi/cpp/initialization).
  - [Initiation aux communications bloquantes](./exercices/mpi/cpp/blocking_com).
  - [Utilisation de MPI_SENDRECV](./exercices/mpi/cpp/sendrecv).
  - [Utilisation des communications non bloquantes MPI_ISEND/MPI_IRECV](./exercices/mpi/cpp/nonblocking_com).
  - [Utilisation de MPI_REDUCE](./exercices/mpi/cpp/reduce_com).
  - [Utilisation de MPI_GATHER](./exercices/mpi/cpp/gather_com).
  - [Création d'un communicateur cartésien](./exercices/mpi/cpp/cartesian_com).
  - [Utilisation du type vector](./exercices/mpi/cpp/type_vector).
  - [Exercice d'échange de cellules fantômes](./exercices/mpi/cpp/gather_matrix).

- Exercices OpenMP
  - [Région parallèle](./exercices/openmp/omp_parallel)
  - [Boucle parallèle](./exercices/openmp/omp_do)
  - [Région critical et atomics](./exercices/openmp/omp_critical)

## Travaux pratiques

- [Equation de diffusion (différences finies)](./tp/diffusion/).
- [Equation d'onde (différences finies)](./tp/onde/).
- [Simulation de particules avec collisions](./tp/marble/).
- [Simulation de vie](./tp/life/).
- [Discrétisation des équations de Maxwell par un schéma de Yee](./tp/maxwell/).
- [Equation en eau peu profonde](./tp/shallow/).
- [Mandelbulb](./tp/mandelbulb/).

## Installation des dépendances

- [Installer le cours sur son ordinateur](https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/installation.html).
- Bibliothèques Python du cours : `pip install -r requirements.txt` (fichier [requirements.txt](./requirements.txt)). h5py et VTK y sont facultatifs : h5py sert au chapitre HDF5 et au TP Kelvin-Helmholtz, VTK au TP Mandelbulb.
- [HDF5 parallèle](https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/installation.html#4-installer-hdf5-en-version-parallele-facultatif).
- [Utilisation de Ruche](./documentation/ruche.md).
