# Exercice 3 : Zone critique

Ce troisième exercice illustre l'utilisté des régions critiques.

## Consignes

1. Compilez le code et exécutez le en l'état. Qu'observez-vous concernant l'affichage dans le terminal et le résultat de la somme.

2. Ajoutez une région *critical* permettant d'améliorer l'affichage. Observez les résultats.

3. Ajoutez une région *critical* permettant de calculer correctement la variable `sum`. Regardez l'impact sur les temps de calcul.

4. Remplacer la région critique par l'utilisation d'un *atomics*. Regarez l'impact sur les temps de calcul.

## Compilation

Pour compiler le code, utilisez le flag `-fopenmp` comme décrit dans le cours :

```bash
# Fortran
gfortran -O3 -fopenmp main.F90 -o executable
# C++
g++ -O3 -fopenmp main.cpp -o executable
```

La compilation génère un fichier exécutable du nom de `executable`.
