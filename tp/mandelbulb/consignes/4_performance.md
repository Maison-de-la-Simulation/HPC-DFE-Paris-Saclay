# IV. Etude de performance

Dans cette dernière partie, nous allons maintenant étudier la performance du code parallèle.
Pour cela nous allons regarder les deux types de passage à l'échelle que sont le *weak* ou *strong scaling*.
Dans chaque cas, vous ferez votre propre étude en utilisant le super-calculateur Ruche.

### 1. Etude de passage à l'échelle faible ou *weak scaling*

Le passage à l'échelle faible consiste à augmenter la taille du problème proportionnellement au nombre de coeurs.
La charge par unité de calcul et donc par coeur reste constante.
Le passage à l'échelle faible étudie la capacité d'un code à simuler un problème plus grand avec plus de ressource en un temps donné.

On utilisera les paramètres suivants :

- discrétisation du domaine: `128 x 128 x 128` par coeur
- iterations: 50
- bailout : 2.0
- power : 8

a) Réaliser l'étude grâce à votre code parallélisé en utilisant 1, 2, 4, 8, 16, 32 coeurs. Rappelons qu'un noeud CPU de Ruche se compose de 2 sockets de 20 coeurs chacun.

**Conseil**: réserver tout le noeud pour votre calcul en utilisant la commande `srun` avec l'option `--ntasks` et `--nodes` même si vous n'utilisez pas tous les coeurs. Vous pouvez aussi spécifier l'option `#SBATCH --exclusive` dans votre script pour être sûr d'avoir tout le noeud pour vous.

b) Calculez la quantité de données à calculer pour chaque coeur (en octet) ainsi que la quantité de données échangées lors des communications globales

c) Tracez la courbe du temps de calcul (en ordonnée) en fonction du nombre de coeurs (en abscisse). Il est préférable de faire plusieurs runs et de moyenner le temps pour lisser les fluctuations.

d) Tracez la courbe d'efficacité (en ordonnée) en fonction du nombre de coeurs (en abscisse). Sur cette courbe, faites apparaître l'efficacité parfaite.

e) Analysez les courbes et commentez les résultats.

### 2. Etude de passage à l'échelle fort ou *strong scaling*

Le passage à l'échelle fort consiste à garder la taille du problème constante et à augmenter le nombre de coeurs.
La charge diminue proportionnellement par l'augmentation du nombre d'unités de calcul.
Le passage à l'échelle forte étudie la capacité d'un code à simuler un même problème avec plus de ressource et donc par exemple dans le but de le résoudre en un temps plus court.

On utilisera les paramètres suivants :

- discrétisation du domaine: `128 x 128 x 128` par coeur
- iterations: 50
- bailout : 2.0
- power : 8

a) Réalisez une étude de passage à l'échelle forte en utilisant 1, 2, 4, 8, 16, 32 coeurs

**Conseil**: réserver tout le noeud pour votre calcul en utilisant la commande `srun` avec l'option `--ntasks` et `--nodes` même si vous n'utilisez pas tous les coeurs. Vous pouvez aussi spécifier l'option `#SBATCH --exclusive` dans votre script pour être sûr d'avoir tout le noeud pour vous.

b) Tracez la courbe du temps de calcul (en ordonnée) en fonction du nombre de coeurs (en abscisse). Sur la courbe vous mettrez le temps total.

c) Tracez la courbe d'efficacité (en ordonnée) en fonction du nombre de coeurs (en abscisse).

d) Analysez les courbes et commentez les résultats.