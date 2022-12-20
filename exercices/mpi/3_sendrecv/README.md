# Fonctionnement des communications point à point bloquantes SEND/RECV

Cet exercice présente le fonctionnement des communications
bloquantes `MPI_Sendrecv`.
On illustre ici son fonctionnement par une communication en anneau.
Chaque processus envoie un message à son voisin de rang supérieur
et reçoit de son voisin de rang inférieur.
On effectue un anneau périodique.

On fait en sorte que chaque rang envoie au rang supérieur la valeur de son propre rang.

## Consignes

1. Prenez le temps de regarder le code dans son intégralité.
Comme pour les codes précédents, on commence par initialiser MPI,
récupérer le nombre total de rangs et récupérer le rang de chaque processus.

2. Il faut maintenant déterminer les rangs des voisins.
On applique un traitement particulier aux rangs situés sur les bords.
On adopte ici des voisins périodiques (pour faire un anneau).
C'est à dire que le rang 0 à pour voisin le rang 1 à droite et le rang N à gauche.
De même, le rang N a pour voisin le rang 0 à droite et N-1 à gauche.
A partir de là, complétez dans le code les lignes suivantes :

```C++
    if (rank == 0) {
        send_rank = ???;
        recv_rank = ???;
    } else if (rank == number_of_ranks - 1) {
        send_rank = ???;
        recv_rank = ???;
    } else {
        send_rank = ???;
        recv_rank = ???
    }
```

## Compilation

```bash
mpif90 main.F90 -o executable
```

```bash
mpic++ main.cpp -o executable
```

## Execution

Il faut au moins 2 processus pour que l'exécution soit intéressante.

```bash
mpirun -np 4 ./executable
```
