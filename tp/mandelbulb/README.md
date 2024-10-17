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

Pour un point situé à la position <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//37fbb2c8364ceb3f7fb9bad29229c6a0.svg?invert_in_darkmode" align=middle width=88.93286489999998pt height=24.65753399999998pt/>, on actualise la valeur en suivant le processus itération suivant :

<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//d7cecea680d5bb51d211e0379a0a257f.svg?invert_in_darkmode" align=middle width=144.33630255pt height=18.312383099999998pt/></p>

Avec <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//f1a9735700937c7c9243c04b42ab621f.svg?invert_in_darkmode" align=middle width=19.79457809999999pt height=26.76175259999998pt/> l'orbite initiale à définir.

La fonction <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/> est définie dans des cordonnées sphériques par :

<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//c1dec99aed2ad7c73156ca9b0ce2ddbf.svg?invert_in_darkmode" align=middle width=444.59462354999994pt height=16.438356pt/></p>

avec :

<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//660ac6728e53aa916735f05a52f6975c.svg?invert_in_darkmode" align=middle width=122.1613569pt height=19.726228499999998pt/></p>

<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//5d45eaaa543bba6c410f981c454b8468.svg?invert_in_darkmode" align=middle width=119.2085334pt height=16.438356pt/></p>

<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2024-2025/.extra//b2a55ef71faa8f9b2c4f954053e211e6.svg?invert_in_darkmode" align=middle width=302.74346519999995pt height=29.58934275pt/></p>

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