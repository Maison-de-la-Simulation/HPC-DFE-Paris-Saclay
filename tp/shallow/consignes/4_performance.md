## IV. Etude de performance

Dans cette dernière partie, nous allons maintenant étudier la performance du code parallèle.
Pour cela nous allons regarder les deux types de passage à l'échelle que sont le *weak* ou *strong scaling*.
Dans chaque cas, vous ferez votre propre étude en utilisant le super-calculateur Ruche.

### 1. Analyse du code avec sorties

Avant de lancer une étude de performance, il est important de comprendre le comportement du code.
Nous allons ici faire tourner le code parallèle avec les sorties activées.

1) Faites tourner le code parallèle en utilisant 10 coeurs avec les paramètres de simulation suivants :

* `L` : 5
* `N` : 128000
* `iterations` : 10000
* `print_period` : 1000
* `output_period` : 100

2) Comparer les résultats avec le code séquentiel pour valider le code parallèle. L'idéal est de faire une comparaison directe des fichiers de sortie en calculant par exemple l'erreur entre les deux fichiers.

3) Rajouter dans le rapport quelques figures de la simulation pour montrer que le code fonctionne correctement.

4) Analyser le temps passé dans chaque partie du code et commentez.

### 2. Etude de passage à l'échelle faible ou *weak scaling*

Le passage à l'échelle faible consiste à augmenter la taille du problème proportionnellement au nombre de coeurs.
La charge par unité de calcul et donc par coeur reste constante.
Le passage à l'échelle faible étudie la capacité d'un code à simuler un problème plus grand avec plus de ressource en un temps donné.
Dans les études de passage à l'échelle faible, on désactivera les sorties sur le disque.

1) Réalisez une étude de passage à l'échelle faible en utilisant 1, 2, 4, 8, 16 et 40 coeurs. Rappelon qu'un noeud CPU de Ruche se compose de 2 sockets de 20 coeurs chacun.

La simulation utilisant un seul coeur (équivalent séquentiel) aura les paramètres suivants :

* `L` : 5
* `N` : 1280000
* `iterations` : 100
* `print_period` : 100
* `output_period` : 0

Pour chaque configuration, il est préférable de faire plusieurs simulations pour obtenir une moyenne du temps de calcul et lisser les fluctuations.

**Astuce** : plutôt que de lancer les simulations à la main, vous pouvez utiliser un script bash ou python pour lancer toutes les simulations en même temps. De même pour l'analyse.

2) Calculez la quantité de données à calculer pour chaque coeur (en octet) ainsi que la quantité de données échangées dans les communications point à point.

3) Tracez la courbe du temps de calcul (en ordonnée) en fonction du nombre de coeurs (en abscisse). Sur la courbe vous mettrez le temps total, le temps dans les communications point à point ainsi que le temps dans les phases de réduction. Utilisez les temps moyens donnés à la fin du code.

4) Tracez la courbe d'efficacité (en ordonnée) en fonction du nombre de coeurs (en abscisse).
Sur cette courbe, faites apparaître l'efficacité parfaite.

5) Analysez les courbes et commentez les résultats.

6) Selon vous, comment évoluerait le passage à l'échelle en augmentant ou en diminuant la discrétisation du domaine `N` ?

### 3. Etude de passage à l'échelle fort ou *strong scaling*

Le passage à l'échelle forte consiste à garder la taille du problème constante et à augmenter le nombre de coeurs.
La charge diminue proportionnellement par l'augmentation du nombre d'unités de calcul.
Le passage à l'échelle forte étudie la capacité d'un code à simuler un même problème avec plus de ressource et donc par exemple dans le but de le résoudre en un temps plus court.
Dans cette étude également on désactivera les sorties sur le disque.

1) Réalisez une étude de passage à l'échelle forte en utilisant 1, 2, 4, 8, 16 et 40 coeurs. 

La simulation utilisant un seul coeur (équivalent séquentiel) aura les paramètres suivants :

* `L` : 5
* `N` : 12800000
* `iterations` : 100
* `print_period` : 100
* `output_period` : 0

Pour chaque configuration, il est préférable de faire plusieurs simulations pour obtenir une moyenne du temps de calcul et lisser les fluctuations.

2) Tracez la courbe du temps de calcul (en ordonnée) en fonction du nombre de coeurs (en abscisse). Sur la courbe vous mettrez le temps total, le temps dans les communications point à point ainsi que le temps dans les phases de réduction. Utilisez les temps moyens donnés à la fin du code.

3) Tracez la courbe d'efficacité (en ordonnée) en fonction du nombre de coeurs (en abscisse).

4) Analysez les courbes et commentez les résultats.

5) Selon vous, comment évoluerait le passage à l'échelle en augmentant ou en diminuant la discrétisation du domaine `N` ?
