## II. Découverte de la machine de calcul

**Question 2.1 - Architecture de la machine parallèle :** Avant de travailler sur la parallélisation du code, il est important de regarder
les propriétés de la machine parallèle que vous allez utiliser.

- a) La première chose à faire est de récupérer ces informations.
  Pour cela, vous pouvez utiliser la commande suivante :

```bash
cat /proc/cpuinfo
```

Cette commande vous donne toutes les informations qui concernent votre processeur.
La ligne `model name` vous permet de récupérer le nom commercial du processeur et de faire une recherche Internet par exemple.
La ligne `cpu cores` vous donne le nombre de coeurs.
Vous noterez que les mêmes informations sont affichées un certain nombre de fois, autant de fois qu'il y a de *threads* pour être exact.
Un *thread* au sens d'Intel est une sous unité de calcul du coeur.
Il y a en général 2 *threads* par coeur qui partagent les ressources mémoires du coeur (à la fois le cache L2 mais aussi la bande passante !).
Lorsque l'on désire utiliser les *threads* comme des unités de calcul, on parle d'*hyperthreading*.

- b) Récupérez le nom du modèle du processeur. Utilisez un moteur de recherche pour trouver la page associée sur le site Intel.

- c) Donnez le nombre de coeurs total et le nombre de *threads* de votre processeur.

- d) Donnez la taille du cache L3.

- e) Sous Gnome, un autre moyen d'avoir des informations sur votre processeur est d'utiliser l'utilitaire graphique `Moniteur système` qui se substitue à la commande `top`.
Combien de CPUs sont affichés ? A quoi correspondent-ils ?
Prenez une capture d'écran et mettez la dans votre rapport.
