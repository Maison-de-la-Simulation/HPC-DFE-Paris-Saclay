# Mandelbulb

Pour en apprendre plus :
- [Mandelbulb wiki page](https://en.wikipedia.org/wiki/Mandelbulb)
- [Daniel White website](https://www.skytopia.com/project/fractal/2mandelbulb.html)
- [Blog Syntopia](http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/)

## Introduction

Les Mandelbulbs sont les versions 3D des fractals de Mandelbrot en 2D.

L'objectif de ce projet est de paralléliser un code séquentiel faisant le calcul d'un Mandelbulb en utilisant MPI.

| ![patterns.png](./img/laser_wakefield.png) |
|:--:|
|<b>Figure 1 - Exemple d'un Mandelbulb (https://en.wikipedia.org/wiki/Mandelbulb)</b>|

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
F(r, theta, phi) = r^n \left( \sin (n \theta) \cos(n\phi), \sin (n \theta) \sin (n\phi), \cos(n \theta)  \right)
$$

avec :

$$
r = \sqrt \left( x^2, y^2, z^2 \right)
$$

$$
\phi = \arctan \left( y / x \right)
$$

$$
\theta = \arctan \left(  \sqrt (x^2 + y^2)  / z \right) = \arccos \left( z / r\right)
$$

## description du projet

### Code séquentiel

### Visualisation

## Consignes

Le projet se découpe en 4 parties. Chaque partie fait l'objet d'un fichier spécifique :

1. [Découvertes du code séquentiel](./1_sequential.md)
2. [Découvertes de la machine](./2_machine.md)
3. [Parallélisation](./3_mpi/md)
4. [Etude de la performance](./4_performance.md)

En plus des consignes, un fichier d'aide est à disposition. Ce fichier sera mis à jour au fur et à mesure de l'avancement du projet et de vos questions.

5. [Aide](./5_aide.md)