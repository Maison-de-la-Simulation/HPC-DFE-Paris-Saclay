# Exercice 7 : Création d'une topologie cartésienne

Cet exercice consiste à mettre en place une topologie cartésienne 2D à
partir des fonctions MPI adéquates.

## Consignes

1. Commencez par regarder les variables déclarées et les variables inialisées :

```fortran
ranks_per_dimension = (/4,3/)
reorganisation      = .true.
periodicity         = (/.true.,.true./)
```

```C++
int ranks_per_direction[2] = {3, 4};
int periodicity[2] = {1, 1};
int reorganisation = 1;
```

La variable `ranks_per_dimension` donne le nombre de rang dans chaque direction.
On doit donc obligatoirement utiliser 4x3 = 12 processus dans ce cas.

2. Compléter la ligne permettant de définir un communicateur cartésien à partir du communicateur par défaut `MPI_COMM_WORLD` :

```fortran
! Creation de la topology cartésienne

Call MPI_???(???, &  ! Ancien communicateur
             ???, &  ! Dimension
             ???, &  ! Nombre de rangs par dimension,
             ???, &  ! Periodicité des rangs voisins,
             ???, &  ! Réorganisation des rangs
             ???, &  ! Nouveau communicateur (cartésien)
             ierror) ! Code d'erreur
```

```C++
// Creation de la topology cartésienne

MPI_???(???, &  // Ancien communicateur
             ???, // Dimension (int)
             ???, // Nombre de rangs par dimension (int *),
             ???, // Periodicité des rangs voisins (int *),
             ???, // Réorganisation des rangs (int)
             ??? // Nouveau communicateur (MPI_Comm *) );
```

Utilisez pour cela les variables déclarées plus haut.

3. Complètez ensuite la ligne permettant de récupérer dans le nouveau communicateur les coordonnées du rang dans la topologie cartésienne :

```fortran
CALL MPI_???(???, &    ! Communicateur cartésien
                    ???, &     ! Rang du processus courant
                    ???, &     ! Dimension
                    ???, &     ! Coordonnées retournées du processus
                    ???)       ! Code d'erreur
```

```C++
MPI_???(???, // Communicateur cartésien (MPI_Comm)
                    ???, // Rang du processus courant (int)
                    ???, // Dimension (int)
                    ??? // Coordonnées retournées du processus (int *)
            );
```

Utilisez pour cela les variables déclarées plus haut.

4. Complètez maintenant les fonctions permettant de récupérer les voisins
de chaque rang via `MPI_CART_SHIFT`. Utilisez pour cela les variables déclarées
plus haut. Récupérer les voisins directs en utilisant un pas positif.
La direction 0 est pour x, la direction 1 pour y.

```fortran
! en -y (rank_neighbors_my), en +y (rank_neighbors_py)
CALL MPI_???(???, &     ! Communicateur cartésien
             ???,                      &     ! Direction
             ???,                      &     ! Déplacement (un pas positif)
             rank_neighbors_my,        &     ! Voisin en -y
             rank_neighbors_py,        &     ! Voisin en +y
             ierror)
```

```C++
// en -y (rank_neighbors_my), en +y (rank_neighbors_py)
MPI_???(???, // Communicateur cartésien (MPI_Comm)
             ???,                      // Direction (int)
             ???,                      // Déplacement (un pas positif, int)
             rank_neighbors_my,        // Voisin en -y (int)
             rank_neighbors_py,        // Voisin en +y (int)
             ierror)
```

5. Compilez le code :

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

6. Exécutez le code avec 12 processus :

```bash
mpirun -np 12 ./executable
```

7. Changez le nombre de processus en prenant soin de changer aussi
le nombre de rangs dans chaque direction `ranks_per_dimension`.

8. Rapatriez sur le rang 0 l'ensemble des rangs de tous les processus.
Utilisez pour cela la bonne communication collective.
Afficher à l'écran la matrice de la topologie cartésienne.

9. Construisez maintenant la carte de la topologie sur tous les processurs en utilisant la fonction `MPI_Cart_coords`.
Afficher à l'écran la matrice de la topologie cartésienne.
Comparez les résultats avec la carte précédente.

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
