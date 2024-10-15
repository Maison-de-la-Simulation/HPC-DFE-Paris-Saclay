## II. Découverte de la machine de calcul

Dans cette deuxième partie, nous allons nous intéresser à la machine de calcul que vous allez utiliser.
Il est important de bien comprendre les caractéristiques de la machine pour pouvoir paralléliser efficacement le code.
Vous allez utiliser le supercalculateur Ruche de l'Université Paris-Saclay.

**Question 2.1 - Architecture de la machine parallèle :** Avant de travailler sur la parallélisation du code, il est important de regarder
les propriétés de la machine parallèle que vous allez utiliser.

- a) La première chose à faire est de récupérer ces informations. En général les calculateurs vous fournissent une documentation sur la machine.
  Il est néanmoins possible de récupérer certaines informations directement sur la machine.

  Pour cela, vous pouvez utiliser la commande suivante sur un noeud de calcul (pas sur la frontale !) :

```bash
lscpu
```

Cette commande vous donne toutes les informations qui concernent votre processeur.
La ligne `Nom de modèle` vous permet de récupérer le nom commercial du processeur et de faire une recherche Internet par exemple.
La ligne `Liste de processeur(s)` vous donne le nombre total de coeurs disponible sur le noeud.

- b) Récupérez le nom du modèle du processeur. Utilisez un moteur de recherche pour trouver la page associée sur le site Intel.

- c) Donnez le nombre de processeurs par noeuds (socket).

- d) Donnez le nombre de coeurs par processeurs (ou coeurs par socket).

- e) Donnez la taille du cache L2 et L3.

- f) Donnez la taille de la mémoire RAM disponible sur le noeud en utilisant la commande `free -h`.