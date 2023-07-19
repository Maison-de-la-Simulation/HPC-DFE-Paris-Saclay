# Exercice 7 : Utilisation des communications collectives MPI_REDUCE

Dans cet exercice, nous intégrons la fonction `f(x) = sin(x)`` de
manière numérique entre 0 et pi en utilisant la méthode des rectangles
Le domaine d'intgération est subdivisé suivant le nombre de processus MPI demandé.

## Consignes

La variable `discretization_per_rank` représente le nombre de `delta` utilisé 
dans chaque rang pour mener l'intégration locale.
La taille du sous-domaine d'un rang est constante et stockée dans `rank_length`.
Les valeurs `min` et `max` représente les bornes du domaine local.

1. Complétez la valeur de `min`, `max`, et `delta`.

2. Complétez l'intérieur de la boucle d'intégration pour effectuer la méthode des rectangles

2. Complétez la ligne MPI destinées au processus de réduction des valeurs partielles
d'intégration dans la variable finale `integration`.

```python
integration = comm.reduce(...)
```

6. Exécutez le code en changeant le nombre de processus `N` :

```bash
mpirun -np N python main.py
```
