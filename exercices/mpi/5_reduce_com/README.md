# Exercice 5 : Utilisation des communications collectives MPI_REDUCE

Dans cet exercice, nous intégrons la fonction f(x) = sin(x) de
manière numérique entre 0 et pi et de manière parallèle
La méthode des rectangles est utilisée pour l'intégration.
Le domaine est subdivisé suivant le nombre de processus MPI demandé.

## Consignes

1. Prenez le temps d'observer l'ensemble du code et de comprendre l'algorithme
d'intégration et sa parallèlisation.
La variable `discretization_per_rank` représente le nombre de points pour
l'intégration partielle au niveau du rang.
La taille du sous-domaine d'un rang est constante et stockée dans `rank_length`.

2. Complétez la ligne MPI destinées au processus de réduction des valeurs partielles
d'intégration dans la variable finale `integration`.

```fortran
Call MPI_???(???,                 & ! valeur local à réduire
                ???,                 & ! valeur finale
                ???,                 & ! Nombre d'éléments
                ???,             & ! Type de donnée échangé
                ???,             & ! Type de réduction
                ???,                   & ! Rang de destination de la réduction
                ???,      & ! Communicateur
                ierror)                ! Code d'erreur
```

5. Compilez le code :

```bash
mpif90 main.F90 -o executable
```

6. Exécutez le code avec le nombre de processus que vous souhaitez :

```bash
mpirun -np 4 ./executable
```

## Compilation

```bash
mpif90 main.F90 -o executable
```

## Execution

```bash
mpirun -np 4 ./executable
