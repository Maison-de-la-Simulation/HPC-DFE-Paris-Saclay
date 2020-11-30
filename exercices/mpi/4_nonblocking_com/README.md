# Exercice 4 : communication point à point non bloquante

Cet exercice met en pratique l'utilisation des communications non-bloquantes `MPI_ISEND` et `MPI_IRECV`.
Pour cela, l'exercice reprend le principe de l'anneau de communication implémenté  dans l'exercice 3.

1. Ajoutez une nouvelle variable `request` pour gérer les requêtes. Vous aurez besoin de 2 identifiants.

2. Ajoutez les appels à `MPI_ISEND`, `MPI_IRECV` et `MPI_WAITALL` en prenant soin de placer les bons paramètres.

3. Compilez et exécutez le code commme dans l'exercice 3.

## Compilation

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

## Execution

```bash
mpirun -np 4 ./executable
