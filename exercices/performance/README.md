# Exercice : Etude de passage à l'échelle

Pour cet exerice, nous allons étudier le passage à l'échelle d'un code parallèle en utilisant le supercalculateur Ruche de l'Université Paris-Saclay. Nous réutilisons le code qui effectue une intégration numérique par la méthode des rectangles.

> [!WARNING]  
> n'oubliez pas de charger Miniforges avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

### Calcul du temps de calcul

On reprend le code de l'exercice sur l'intégration numérique avec MPI.

1. Ajoutez le calcul du temps de calcul total (englobant la boucle d'intégration et la réduction) en utilisant les fonctions `MPI.Wtime()` avant et après la section de code concernée.

2. Utilisez la fonction `comm.reduce` pour récupérer le temps de calcul minimum, maximum et moyen parmi tous les processus.

3. Affichez ces valeurs depuis le rang 0.

### Passage à l'échelle faible

1. Réalisez une étude de passage à l'échelle faible en utilisant 1, 2, 4, 8, 16 et 20 coeurs pour une discrétisation de 320000000 de mailles pour 1 coeur. Pour vous aidez vous pouvez utiliser le script `weak.slurm` fourni.

2. Tracer la courbe d'efficacité en fonction du nombre de coeurs. Vous pouvez utiliser le script `weak_scalability.py` fourni.

3. Commentez les résultats.

### Passage à l'échelle fort

1. Réalisez une étude de passage à l'échelle fort en utilisant 1, 2, 4, 8, 16 et 20 coeurs pour une discrétisation de 320000000 de mailles pour 1 coeur. Pour vous aidez vous pouvez utiliser le script `strong.slurm` fourni.

2. Tracer la courbe d'efficacité en fonction du nombre de coeurs. Vous pouvez utiliser le script `strong_scalability.py` fourni.

3. Commentez les résultats.

