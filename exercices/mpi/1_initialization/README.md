# Exercice 1 : Premier programme MPI

Ce premier exercise a simplement pour but de créer un premier programe MPI
capable de récupérer le nombre total de rangs et d'afficher pour chaque rang leur numéro dans le communicateur par défaut (`MPI_COMM_WORLD`).

## Consignes

1. Complétez la ligne permettant d'initaliser MPI avec la fonction correspondante dans le cours : MPI_INIT

2. Complétez maintenant la ligne permettant de récupérer le nombre total de processus MPI utilisés pour ce programme : `MPI_COMM_SIZE`. Ajoutez-y les bons arguments.

3. Complètez maintenant la ligne permettant de récupérer le rang de chaque processus : `MPI_COMM_RANK`. Pour rappel, lorsqu'un programme MPI est exécuté, il est dupliqué suivant le nombre de processus demandé. Chaque processus exécute le code indépendamment.

4. Complètez la dernière ligne sur la finalisation de MPI : `MPI_FINALIZE`

5. Compilez le code :

```bash
mpif90 main.F90 -o executable
```

6. Exécutez le code en faisant varier le nombre de processus

```bash
# Exemple pour 4 processus
mpirun -np 4 ./executable
```

7. Vous obtenez normalement une réponse de chaque processus comme cela :

```
 Je suis le rang   0 /  3
 Je suis le rang   1 /  3
 Je suis le rang   2 /  3
 Je suis le rang   3 /  3

```

8. Comme pour le dernier exercice sur OpenMP, vous allez maintenant déclarer 4 tableaux A, B, C et D :

```fortran
real(8), dimension(:), allocatable :: A, B, C, D
```

9. Faites en sorte que le nombre d'éléments dans les tableaux diminuent proportionnelement au nombre de rangs :
```fortran
local_N = N / number_of_ranks
allocate(A(local_N),B(local_N),C(local_N),D(local_N))
```

10. Initialisez ces tableaux :
```fortran
Do i = 1,local_N
    B(i) = cos(i*2.0)
    C(i) = sin(i*4.0)
    D(i) = cos(i*1.0)**2
End do
```

11. Implémentez maintenant une boucle de calcul de la complexité de votre choix, par exemple :
```fortran
Do i = 1,local_N
    A(i) = B(i) + C(i)*D(i)
End do
```

12. Implémenter le calcul du temps passé dans la boucle de calcul précédente en utilisant `MPI_WTIME()`. Faites en sorte que chaque processus affiche le temps en même temps que son rang.

13. Initilisez N avec une grande valeur et faites varier le nombre de processus MPI à l'execution.
Observez l'effet sur le temps de calcul de chaque rang.

## Compilation

Pour compiler le code, utilisez le wrapper MPI comme décrit dans le cours :

```bash
# Fortran
mpif90 -O3 main.F90 -o executable
# C++
mpic++ -O3 main.cpp -o executable
```

La compilation génère un fichier exécutable du nom de `executable`.

## Exécution

Pour exécuter le code avec MPI, utilisez le runtime MPI.
L'argument `-np 4` désigne le nombre de processus à utiliser, ici 4.

```bash
mpirun -np 4 ./executable
