# Exercice : Etude de passage à l'échelle

Pour cet exerice, nous allons étudier le passage à l'échelle d'un code parallèle en utilisant le supercalculateur Ruche de l'Université Paris-Saclay. Nous réutilisons le code qui effectue une intégration numérique par la méthode des rectangles.

## Consignes

### Passage à l'échelle faible

1. Réalisez une étude de passage à l'échelle faible en utilisant 1, 2, 4, 8, 16 et 20 coeurs pour une discrétisation de 320000000 de mailles pour 1 coeur. Pour vous aidez vous pouvez utiliser le script `weak.slurm` fourni.

2. Tracer la courbe d'efficacité en fonction du nombre de coeurs. Commentez les résultats.

### Passage à l'échelle fort

1. Réalisez une étude de passage à l'échelle fort en utilisant 1, 2, 4, 8, 16 et 20 coeurs pour une discrétisation de 320000000 de mailles pour 1 coeur. Pour vous aidez vous pouvez utiliser le script `strong.slurm` fourni.

2. Tracer la courbe d'efficacité en fonction du nombre de coeurs. Commentez les résultats.

**Warning**: n'oubliez pas de charger l'environnement Python avant d'exécuter le code.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```
