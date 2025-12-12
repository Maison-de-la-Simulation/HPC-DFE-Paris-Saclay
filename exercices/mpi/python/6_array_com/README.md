# Exercice 6 - Communication de tableaux

Cet exercice met en pratique non plus la communication d'un simple entier mais celle d'un tableau de type numpy.

> [!WARNING]  
> n'oubliez pas de charger miniforge avant de commencer cet exercice.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

1. Complétez la première communication bloquante pour l'échange d'un tableau d'entier

2. Complétez maintenant la deuxième communication pour l'échange d'un tableau de float

3. Exécutez le code en utilisant 4 processus :

```bash
mpirun -np 4 ./executable
```