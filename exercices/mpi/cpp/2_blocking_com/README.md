# Fonctionnement des communications point à point bloquantes SEND/RECV

Ce deuxième exercise a pour but d'illustrer l'utilisation des communications bloquantes point à point.
Dans ce code, le processus MPI de rang 1 envoie un message au processus
de rang 2.

## Consignes

1. Prenez le temps de regarder le code dans sa globalité.
L'appel à `MPI_INIT` permet d'initialiser le communicateur `MPI_COMM_WORLD`.
Vous remarquez que l'on récupère les rangs dans un premier temps grâce à `MPI_COMM_RANK`.
La récupération des rangs permet d'effectuer des actions différenciées pour
chaque processus par l'intermédiaire d'une simple condition `if`.
Ici, l'envoi est effectué par le rang 1 et la réception par le rang 2.

2. Complétez la fonction qui permet au rang 1 d'envoyer la variable `message` de
 type entier (`integer`) au rang 2.

```fortran
Call MPI_???(???,                  & ! La partie du tableau à envoyer
              ???,                        & ! Le nombre d'éléments
              ???,              & ! Le type de donnée utilisé
              ???,                        & ! Le rang du destinataire
              ???,                      & ! tag de la communication
              ???,           & ! Le communicateur
              ierror)                     ! Code d'erreur
```

3. Complèter maintenant la fonction qui permet de réceptionner le
message venant du rang 1.

```fortran
Call MPI_???(???,                  & ! La variable qui récupère le message
              ???,                        & ! Le nombre d'éléments à recevoir
              ???,                  & ! Le type de donnée envoyé
              ???,                        & ! Le rang de l'expéditeur
              ???,                      & ! tag de la communication
              ???,           & ! Le communicateur
              MPI_STATUS_IGNORE,        & ! Statut de la communication
              ierror)                     ! Code d'erreur
```

4. Compiler le code :

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

5. Exécuter le code avec au moins 3 processus :

```bash
mpirun -np 4 ./executable
```

## Compilation

```bash
mpif90 main.F90 -o executable
```

## Execution

Il faut au moins 3 processus pour que le programme fonctionne.

```bash
mpirun -np 4 ./executable
```
