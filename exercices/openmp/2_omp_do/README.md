# Exercice 2 : Parallélisation d'une boucle avec OpenMP

Ce deuxième exercice a pour but de créer de rendre parallèle une boucle de calcul avec OpenMP
tout en gérant les zones séquentielles.

## Consignes

1. Compilez et exécutez ce code séquentiel pour l'instant.

2. Placez la région parallèle en la faisant commencer le plus haut possible dans le code.
   
3. Spécifiez que par défaut les variables sont partagées.

4. Précisez pour les variables dont vous pensez que c'est nécessaire la clause `private` et `firstprivate`.

5. Rajoutez devant les boucles la directive permettant de distribuer la charge sur les threads de la région parallèle.

6. Précisez que le scheduler est décidé à l'exécution.

7. Remplacer les fonctions de temps par les fonctions d'OpenMP.

8. Si les appels aux fonctions de temps sont dans la région parallèle, que faut-il rajouter pour avoir les temps affichés uniquement par le thread `master` ?

9. Compilez le code et exécutez-le. N'oubliez pas de préciser avant l'exécution les variables d'environnement nécessaires au bon fonctionement du code OpenMP (quel *scheduler* et combien de *threads* à utiliser. Commencez par utilisez un *scheduler* de type `static` sur des *chunks* de taille 100. Demandez 4 *threads* OpenMP.

10. Faites varier le type de *scheduler* et la taille des *chunks* tout en observant les conséquences sur le temps de calcul.

11. Tout en gardant votre *scheduler* constant, faites varier le nombre de *threads* OpenMP tout en observant les conséquences sur le temps de calcul.

## Compilation

Pour compiler le code, utilisez le flag `-fopenmp` comme décrit dans le cours :

```bash
# Fortran
gfortran -O3 -fopenmp main.F90 -o executable
# C++
g++ -O3 -fopenmp main.cpp -o executable
```

La compilation génère un fichier exécutable du nom de `executable`.
