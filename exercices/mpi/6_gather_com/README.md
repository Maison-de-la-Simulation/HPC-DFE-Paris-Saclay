# Exercice 6 : Utilisation de MPi_Gather dans tous ses états (ou presque)

Cet exercice met en pratique l'utilisation de `MPI_Gather` et sa variante `MPI_Gatherv`.

Pour cela, chaque rang MPI alloue et initie un tableau `double * array` dont la taille est déterminée par une variable aléatoire `size`.
La taille de `array` sur chaque rang est entre 1 et 100 :
```C++
// The size is drawn between 1 and 100
int size = rand() % 99 + 1;
```
L'initialisation du tableau est également aléatoire :
```C++
for (int i = 0; i < size ; i++) {
    array[i] = drand48() * (rank+1);
}
```

## Consignes

1. La première chose à faire est de rappatrier sur le rang 0 le nombre d'éléments stockés dans le tableau de chaque rang MPI.
Pour ce faire on déclare d'abord le tableau d'entiers dont on aura besoin :
```C++
int * local_size = new int[number_of_ranks];
```
Remplissez la fonction MPI permettant de compléter le tableau d'entier `local_size`.

2. On calcul ensuite la somme des éléments de tous les processus sur le rang 0.
On déclare d'abord la variable qui contiendra cette valeur :
```C++
int total_size = 0;
```
Complétez maintenant la portion de code permettant de de calculer cette valeur sur le rang 0.

3. L'étape suivante est la construction du tableau des déplacements pour la fonction `MPI_Gatherv`.
Pour rappel le déplacement est l'endroit où commence la contribution de chaque rang au tableau global.
On commence par déclarer le tableau des déplacements :
```C++
int * displacement = new int [number_of_ranks];
```
Complétez enssuite le code permettant de remplir ce tableau.

4. On effectue maintenant l'opération de rappatriement des données sur le rang 0.
On déclare d'abord le tableau qui contiendra tous les éléments :
```C++
double * global_array = new double [total_size];
```
Complétez ensuite la fonction MPI permettant de faire le rappatriement.

5. La somme des éléments est ensuite faite sur le rang 0. Afin de vérifier que ce résultat est bon, effectuez la même somme en utilisant cette fois une opération de réduction `MPI_Reduce`.

6. Compilez le code :

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

6. Exécutez le code avec le nombre de processus que vous voulez :

```bash
mpirun -np ? ./executable
```
Vérifiez que vous obtenez le même résultat entre les deux façon de sommer les éléments.

## Compilation

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

## Execution

```bash
mpirun -np 12 ./executable
```
