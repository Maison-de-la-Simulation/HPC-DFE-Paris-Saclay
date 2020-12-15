# Exercice 1 : Premier programme MPI

Ce premier exercise a simplement pour but de créer un premier programe MPI
capable de récupérer le nombre total de rangs et d'afficher pour chaque rang leur numéro dans le communicateur par défaut (`MPI_COMM_WORLD`).

## Consignes

1. Complétez la ligne permettant d'initaliser MPI avec la fonction correspondante dans le cours : `MPI_INIT` en fortran ou `MPI_Init` en C/C++.

2. Complétez maintenant la ligne permettant de récupérer le nombre total de processus MPI utilisés pour ce programme : `MPI_COMM_SIZE`. Ajoutez-y les bons arguments.

3. Complètez maintenant la ligne permettant de récupérer le rang de chaque processus : `MPI_COMM_RANK` en Fortan ou `MPI_Comm_rank` en C/C++.
   Pour rappel, lorsqu'un programme MPI est exécuté, il est dupliqué suivant le nombre de processus demandé.
   Chaque processus exécute le code indépendamment.

4. Complètez la dernière ligne sur la finalisation de MPI : `MPI_FINALIZE` en Fortran ou `MPI_Finalize` en C.

5. Compilez le code.
En Fortran :
```bash
mpif90 main.F90 -o executable
```
En C++ :
```bash
mpic++ main.cpp -o executable
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

8. Comme pour le dernier exercice sur OpenMP, vous allez maintenant déclarer et allouer 3 tableaux A, B et C :

```fortran
real(8), dimension(:), allocatable :: A, B, C

allocate(A(local_N),B(local_N),C(local_N))
```

```C++
double * A = new double[local_N];
double * B = new double[local_N];
double * C = new double[local_N];
```

9. Faites en sorte que le nombre d'éléments dans les tableaux diminuent proportionnelement au nombre de rangs :
```fortran
local_N = N / number_of_ranks
```

```C++
int local_N = N / number_of_ranks;
```

10. Initialisez ces tableaux :
```fortran
Do i = 1,local_N
    A(i) = sin(i*4.0)
    B(i) = sin(i*4.0)
    C(i) = cos(i*1.0)**2
End do
```

```C++
for(int i = 0 ; i < local_N ; i++) {
    A[i] = sin(4.0*i);
    B[i] = pow(sin(10.0*i),2);
    C[i] = cos(5.0*i);
}
```

11. Implémentez maintenant une boucle de calcul de la complexité de votre choix, par exemple :
```fortran
Do i = 2,local_N-1
    A(i) = B(i) - C(i) + (B(i-1) + B(i+1) + 4*B(i))
end do
```

```C++
for(int i = 1 ; i < local_N-1 ; i++) {
    A[i] = B[i] - C[i] + (B[i-1] + B[i+1] + 4*B[i]);
}
```

12. Implémenter le calcul du temps passé dans la boucle de calcul précédente en utilisant `MPI_WTIME()` (`MPI_Wtime` en C/C++).
Faites en sorte que chaque processus affiche le temps en même temps que son rang.

13. Initilisez `N` avec une grande valeur et faites varier le nombre de processus MPI à l'execution.
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
