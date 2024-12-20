# Mandelbulb

Pour en apprendre plus :
- [Mandelbulb wiki page](https://en.wikipedia.org/wiki/Mandelbulb)
- [Daniel White website](https://www.skytopia.com/project/fractal/2mandelbulb.html)
- [Blog Syntopia](http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/)

## Introduction

Les Mandelbulbs sont les versions 3D des fractals de Mandelbrot.

L'objectif de ce projet est de paralléliser un code séquentiel faisant le calcul d'un Mandelbulb en utilisant MPI.

| ![patterns.png](https://upload.wikimedia.org/wikipedia/commons/thumb/a/a0/Power_8_mandelbulb_fractal_overview.jpg/1920px-Power_8_mandelbulb_fractal_overview.jpg) |
|:--:|
|<b>Figure 1 - Exemple d'un Mandelbulb d'ordre 8 par Ondřej Karlík (https://en.wikipedia.org/wiki/Mandelbulb)</b>|


## Principes

Pour ce projet, nous utilisons formule bien connue de White and Nylander's.

La construction d'un Mandelbulb est un processus itératif qui s'effectue pour un ensemble de point d'un espace donné, donc autrement dit sur une grille 3D ou un tenseur 3D.

Pour un point situé à la position $C = \left( x, y, y \right)$, on actualise la valeur en suivant le processus itératif suivant :

$$
V^{n+1} = F\left( V^{n} \right) + C
$$

Avec $V^0$ l'orbite initiale à définir.

La fonction $F$ est définie dans des cordonnées sphériques par :

$$
F(r, \theta, \phi) = r^n \left( \sin{(n \theta)} \cos(n\phi), \sin (n \theta) \sin (n\phi), \cos(n \theta)  \right)
$$

avec :

$$
r = \sqrt{ x^2 + y^2 + z^2 }
$$

$$
\phi = \arctan \left( y / x \right)
$$

$$
\theta = \arctan \left(  \sqrt{x^2 + y^2}  / z \right) = \arccos \left( z / r \right)
$$

Ici, $n$ est l'ordre du Mandelbulb.

Comme pour la version 2D, Le processus itératif s'arrête lorsque la valeur de $V$ dépasse une certaine valeur.

Le seuil utilisé dans de nombreux scénarios est $r > 2$.

## Description du projet

### Code séquentiel

Le code séquentielle est présente dans le dossier `sequential/mandelbulb.py`.

Il se compose de plusieurs parties :

1) **Input parameters :** définition des paramètres d'entrée
2) **Command line arguments :** gestion des arguments en ligne de commande
3) **Initialization :** initialisation des variables et des conditions initiales
4) **Terminal output summary :** affichage des informations de sortie dans le terminal
5) **Mandelbulb computation :** calcul du Mandelbulb
6) **Compute the volume of the Mandelbulb :** calcul du volume du Mandelbulb
7) **Save the domain array using the vtk format :** sauvegarde du Mandelbulb au format VTK

### Requirements

Le code nécessite les bibliothèques suivantes :

- `numpy` : numpy est une bibliothèque de calcul numérique en Python.
- `time` : time est une bibliothèque standard de Python qui permet de gérer le temps.
- `math` : math est une bibliothèque standard de Python qui permet de gérer les fonctions mathématiques.
- `argparse` : argparse est une bibliothèque standard de Python qui permet de gérer les arguments en ligne de commande.
- `vtk` : VTK est une bibliothèque de visualisation 3D. Elle est utilisée pour sauvegarder le Mandelbulb au format VTK permettant de le visualiser avec des logiciels comme Paraview.

### Fonctionnement du code

La version séquentielle du code tourne avec ses paramètres par défaut en faisant :

```bash
python mandelbulb.py
```

A la fin de l'exécution, le code produit un fichier au format vtk au nom de `mandelbulb.vtk`

Les paramètres par défaut peuvent être modifiés directement dans le code ou en ligne de commande. Vous pouvez obtenir la liste des arguments en faisait :

```bash
python mandelbulb.py -h
```

Voici la liste des arguments disponibles :

- `--domain_size` : résolution du domaine dans chaque direction
- `--domain_min` : coordonnées minimales du domaine
- `--domain_max` : coordonnées maximales du domaine
- `--iterations` : nombre d'itérations
- `--initial_position` : position initiale de l'orbite
- `--bailout` : valeur de sortie pour la génération du Mandelbulb
- `--power` : ordre du Mandelbulb

Par exemple :

```bash
python mandelbulb.py --domain_size 100 --domain_min -2 -2 -2 --domain_max 2 2 2 --iterations 100 --initial_position 0 0 0 --bailout 2 --power 8
```

### Visualisation

Le code séquentiel produit à la fin un fichier au format VTK. Ce format, très classique en visualisations scientifique, peut être ensuite ouvert avec le logiciel de visualisation Paraview. Comment utiliser Paraview fera l'object de la première partie du projet.

## Consignes

Le projet se découpe en 4 parties. Chaque partie fait l'objet d'un fichier spécifique :

1. [Découvertes du code séquentiel](./consignes/1_sequential.md)
2. [Découvertes de la machine](./consignes/2_machine.md)
3. [Parallélisation](./consignes/3_mpi/md)
4. [Etude de la performance](./consignes/4_performance.md)

En plus des consignes, un fichier d'aide est à disposition. Ce fichier sera mis à jour au fur et à mesure de l'avancement du projet et de vos questions.

5. [Aide](./5_aide.md)

## Notation

Vous serez noté sur le code et la construction d'un rapport de projet.

Chaque question rapporte des points selon la difficulté. Même si le code ne fonctionne pas, j'analyse l'ensemble du code étape par étape pour voir si vous avez compris les concepts.

Le rapport permet de répondre aux quelques questions posées dans les consignes. Il doit être clair et concis. Il n'est pas nécessaire de faire une introduction sur le sujet, répondez simplement aux questions posées. Le format (tex, docx, etc) est libre. L'idée c'est efficacité et clarté.

Vous rendrez votre projet en m'envoyant un email avec un lien vers une archive zip ou tar contenant : 
- le code source parallèle (fichier `mandelbulb.py`)
- le rapport

Attention, il ne faut pas y mettre :
- résultats de simulations
- autres scripts

## Date de rendu

La date est à déterminer. Elle sera communiquée ultérieurement.