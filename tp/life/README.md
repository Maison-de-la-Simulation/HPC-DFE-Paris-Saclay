# Jeu de la vie avancé

## Présentation du concept

[Le jeu de la vie](https://fr.wikipedia.org/wiki/Jeu_de_la_vie) est un simulateur
simplifié de comportement cellulaire inventé
par le mathématicien John Horton Conway en 1970.
Le jeu présenté ici s'inspire de l'idée du jeu de la vie même si les propriétés
mathématiques du jeu d'origine ne sont pas conservées (Turing complet par exemple).
Le but de ce TP est d'utiliser un simulateur de vie ludique à 3 espèces en
se basant sur des règles simples d'interaction entre ces dernières.
Ce simulateur ne se veut en aucun cas une description réaliste ou un modèle scientifique
contrairement à d'autres TPs de ce cours.

Le jeu est donc composé de 3 espèces qui évolueront librement dans un domaine
fermé suivant des règles de survie propre à chacune d'elles.
Les 3 espèces sont les suivantes:
- la végétation
- les lapins
- les renards
L'idée principale est d'avoir 3 types d'espèce classique que sont les végétaux,
les proies herbivores et les prédateurs carnivores.

### Le domaine

Chaque espèce est modélisée par une grille cartésienne.
Le domaine est donc découpé en cellules.
Chaque cellule représente la densité de végétaux
ou la quantité d'individus dans ce sous-espace.
Nous raisonnons donc de manière très schématique.
Les individus sont partiellement modélisés individuellement
au sein d'une cellule puis se reconfondent avec l'ensemble.
Il n'y a pas de suivi individuel au cours de toute la simulation.
Il s'agit avant tout d'une approche statistique.
Les individus ne peuvent se déplacer que d'une cellule à une autre.

### Fonctionnement des végétaux

Les végétaux obéissent aux comportements décrits dans le diagramme suivant.

<img src="../../support/materiel/life_plants_behavior.png" height="400">

### Fonctionnement des lapins

### Fonctionnement des loups
