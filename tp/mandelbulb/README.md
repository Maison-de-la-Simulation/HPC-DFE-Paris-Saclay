# Mandelbulb

Pour en apprendre plus :
- [Mandelbulb wiki page](https://en.wikipedia.org/wiki/Mandelbulb)
- [Daniel White website](https://www.skytopia.com/project/fractal/2mandelbulb.html)
- [Blog Syntopia](http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/)

## Introduction

Les Mandelbulbs sont les versions 3D des fractals de Mandelbrot en 2D.

L'objectif de ce projet est de paralléliser un code séquentiel faisant le calcul d'un Mandelbulb en utilisant MPI.

| ![patterns.png](https://upload.wikimedia.org/wikipedia/commons/thumb/a/a0/Power_8_mandelbulb_fractal_overview.jpg/1920px-Power_8_mandelbulb_fractal_overview.jpg) |
|:--:|
|<b>Figure 1 - Exemple d'un Mandelbulb d'ordre 8 par Ondřej Karlík (https://en.wikipedia.org/wiki/Mandelbulb)</b>|


## Principes

Pour ce projet, nous utilisons formule bien connue de White and Nylander's.

La construction d'un Mandelbulb est un processus itératif qui s'effectue pour un ensemble de point d'un espace donné.

Pour un point situé à la position $C = \left( x, y, y \right)$, on actualise la valeur en suivant le processus itération suivant :

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

Le code séquentiel est présent dans le dossier `sequential/mandelbulb.py`.

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

### Visualisation

## Consignes

Le projet se découpe en 4 parties. Chaque partie fait l'objet d'un fichier spécifique :

1. [Découvertes du code séquentiel](./1_sequential.md)
2. [Découvertes de la machine](./2_machine.md)
3. [Parallélisation](./3_mpi/md)
4. [Etude de la performance](./4_performance.md)

En plus des consignes, un fichier d'aide est à disposition. Ce fichier sera mis à jour au fur et à mesure de l'avancement du projet et de vos questions.

5. [Aide](./5_aide.md)