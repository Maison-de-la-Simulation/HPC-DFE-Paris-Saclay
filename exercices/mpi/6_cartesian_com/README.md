# Exercice 6 : Création d'une topologie cartésienne

Cet exercice consiste à mettre en place une topologie cartésienne 2D à
partir des fonctions MPI adéquates.

## Consignes

1. Commencez par regarder les variables déclarées et les variables inialisées :

```fortran
ranks_per_dimension = (/4,3/)
reorganisation      = .true.
periodicity         = (/.true.,.true./)
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

Utilisez pour cela les variables déclarées plus haut.

3. Complètez ensuite la ligne permettant de récupérer dans le nouveau communicateur les coordonnées du rang dans la topologie cartésienne :

```fortran
CALL MPI_???(???, &    ! Communicateur cartésien
                    ???, &     ! Rang du processus courant
                    ???, &     ! Dimension
                    ???, &     ! Coordonnées retournées du processus
                    ???)       ! Code d'erreur
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

5. Compilez le code :

```bash
mpif90 main.F90 -o executable
```

6. Exécutez le code avec 12 processus :

```bash
mpirun -np 12 ./executable
```

7. Changez le nombre de processus en prenant soin de changer aussi
le nombre de rangs dans chaque direction `ranks_per_dimension`.

8. Question bonus : rapatriez sur le rang 0 l'ensemble des rangs et
coordonnées de tous les processus.
Utilisez pour cela la bonne communication collective.
Afficher à l'écran la matrice de la topologie cartésienne.

## Compilation

```bash
mpif90 main.F90 -o executable
```

## Execution

```bash
mpirun -np 12 ./executable
```
