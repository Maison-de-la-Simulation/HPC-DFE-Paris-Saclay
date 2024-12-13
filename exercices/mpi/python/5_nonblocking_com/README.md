# Exercice 5 : communications point à point non bloquantes

Cet exercice met en pratique l'utilisation des communications non-bloquantes `isend` et `irecv`.
Pour cela, l'exercice reprend le principe de l'anneau de communication implémenté  dans l'exercice précédent.

1. Ajoutez les appels à `isend` et `irecv` en prenant soin de placer les bons paramètres.

2. Ajoutez les appels à `wait` pour finaliser les communications et récupérer le message envoyé.

3. Executez le code comme pour l'exercice précédent en variant le nombre de processus `N` :

```bash
mpirun -np N ./executable
```

**Warning**: n'oubliez pas de charger l'environnement Python avant d'exécuter le code.

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```