# Exercice 8 : Application du type dérivé vector

Cet exercice consiste à utiliser le type dérivé vector.

Dans cet exercice chaque rang MPI possède un tableau 2D du nom de `array`.
Pour rappel, en C++, c'est toujours l'indice le plus interne qui correspond à l'indice le plus rapide, c'est à dire à la continuité des données stockées en mémoire.

<img src="../../../support/materiel/array.svg" height="400">

Chaque rang va communiquer au rang suivant  sa colonne la plus à droite en utilisant le concept
d'anneau périodique.
Le rang qui réceptionne place la colonne reçue dans sa colonne la plus à gauche.

## Consignes

1. Commencez par regarder les variables déclarées et les variables initialisées :

```C++
int nx = 10;
int ny = 16;

int array[ny][nx];
```

2. Compléter les lignes permettant de déclarer le type vector du nom de `column`.

3. Complétez les fonctions permettant l'envoi et la réception des données.

4. Compilez le code :

```bash
mpic++ main.cpp -o executable
```

5. Exécutez le code avec 12 processus :

```bash
mpirun -np 12 ./executable

Et vérifiez les résultats
