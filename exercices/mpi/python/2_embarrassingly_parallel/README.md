# Exercice 2 : Parallelisme parfait

Ce deuxième exercice va mettre en pratique la notion de partage du travail dans le cadre d'un parallélisme parfait.
Chaque rang va recevoir une quantité de travail indépendante à effectuer.
La quantité de travail est bien répartie entre les processus.
Le temps total sera mesuré avec la fonction MPI correspondante.
Pour cela, nous repartons de l'exercice précédent. 

1. Calculez le nombre d'éléments sur chaque processus `local_N` en fonction du nombre total `N` et du nombre de rangs `number_of_ranks`.

2. Nous allons maintenant créer 3 tableaux numpy A, B et C initialisés aléatoirement de taille `local_N`

```python
A = np.random.rand(local_N)
B = np.random.rand(local_N)
C = np.random.rand(local_N)
```

Après cette étape, nous effectuons une boucle de calcul sur ces tableaux :

```python
A = 2.9*B + 5.2*C + 265.7*B*C + B**2 + C**2
```

3. Utilisez la fonction MPI du temps `MPI.Wtime()` pour calculer le *timer* avant et après la boucle de calcul.

4. Testez votre code en utilisant 1, 2 puis 4 processus jusqu'au maximum de votre ordinateur et observez l'effet sur le temps de calcul

```bash
# Exemple pour N processus
mpirun -np N python main.py
```

5. Avec 4 processus, choisissez un nombre important d'éléments pour faire durer l'exécution plusieurs secondes 
et observer l'effet sur les tâches de votre système (via `top` ou `htop` par exemple)
