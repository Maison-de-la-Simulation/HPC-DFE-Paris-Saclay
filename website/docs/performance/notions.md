# 1. Déséquilibre de charge et parallélisme parfait

## Le déséquilibre de charge

Déséquilibre de charge (*load imbalance*)
:   Situation où les processus MPI d'une même exécution n'ont pas la même
    quantité de travail à effectuer (de calcul, par exemple). Leurs temps
    de traitement diffèrent, ce qui entraîne des **phases d'attente** non
    désirées.

Les processus qui ont fini leur calcul doivent attendre les autres avant
de pouvoir communiquer :

--8<-- "performance/desequilibre.svg"

!!! warning "L'attente est une perte"

    De l'attente n'est ni plus ni moins que de la **ressource de calcul
    gaspillée** : pendant ce temps, des cœurs réservés ne calculent rien.

## Le parallélisme parfait

Parallélisme parfait (*embarrassingly parallel*)
:   Un parallélisme qui ne contient **aucune zone séquentielle**, **aucun
    échange**, **aucune dépendance** et **aucune synchronisation** entre
    les processus.

Chaque processus travaille alors de façon totalement indépendante. C'est
le cas idéal, qui sert de référence pour juger la performance d'un code
réel. L'exercice 2 du chapitre MPI,
[Parallélisme parfait](../mpi/premier-programme.md#notion-de-parallelisme), en est
un exemple.
