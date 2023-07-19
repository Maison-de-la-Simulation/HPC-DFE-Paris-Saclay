# Exercice 1 : Premier programme MPI

Ce premier exercise a simplement pour but de créer un premier programe MPI en Python
capable de récupérer le nombre total de rangs et d'afficher pour chaque rang leur numéro dans le communicateur par défaut (`MPI_COMM_WORLD`).

## Consignes

1. Complétez la ligne permettant d'initaliser MPI avec la fonction python correspondante : `MPI.Init()`. 

Pour rappelle, en Python cette étape est en fait automatique. Nous l'avons ici désactivé à la main pour l'exercice en rajoutant :

```python
import mpi4py
mpi4py.rc.initialize = False  
mpi4py.rc.finalize = False
```

En C, C++ ou Fortran, elle serait obligatoire.

2. Compléter la ligne permettant de récupérer le communicateur global ou communicateur par défaut: `comm = MPI.COMM_WORLD`

2. Complétez maintenant la ligne permettant de récupérer le nombre total de processus MPI utilisés pour ce programme : `comm.Get_size()`.

3. Complètez maintenant la ligne permettant de récupérer le rang de chaque processus : `comm.Get_rank()`.
   Pour rappel, lorsqu'un programme MPI est exécuté, il est dupliqué suivant le nombre de processus demandé.
   Chaque processus exécute le même code indépendamment.

4. Complètez la dernière ligne sur la finalisation de MPI : `MPI.Finalize()`.


5. Avec Python, il n'est pas nécessaire de compiler le code mis cette étape serait obligatoire en C ou Fortran. Exécutez le code en faisant en remplaçant `N` par le nombre de processus que vous souhaitez utiliser :

```bash
# Exemple pour N processus
mpirun -np N python main.py
```

Vous obtenez normalement une réponse de chaque processus comme cela :

```
 Je suis le rang   0 /  3
 Je suis le rang   1 /  3
 Je suis le rang   2 /  3
 Je suis le rang   3 /  3

```
