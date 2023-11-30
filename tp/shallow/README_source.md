# Equation en eau peu prodonde ou équation de Saint-Venant

Pour en apprendre plus :
- [Excellent cours de P.-Y. Lagrée](http://www.lmm.jussieu.fr/~lagree/COURS/MFEnv/code_C_saintvenant.pdf)
- [Shallow Water Equations](https://en.wikipedia.org/wiki/Shallow_water_equations)

## Objectifs

L'objectif de ce projet est de résoudre en parallèle grâce à MPI l'équation en eau peu profonde conservatrice en 1D.

## Principes

Les équations en eau peu profonde sont des équations hyperboliques non linéaires qui approximent l'évolution de la hauteur d'eau d'un canal, d'une retenue, des marées, etc. 

On s'intéresse par simplicité ici à la forme conservative de l'équation en eau peu profonde en 1D.

$x$ est l'axe spatial et $t$ le temps.

On a alors :

$$
\frac{\partial h}{\partial t} + \frac{\partial q}{\partial x} = 0
$$

$$
\frac{\partial q}{\partial t} + \frac{\partial}{\partial x} \left( \frac{q^2}{h} + \frac{1}{2} g h^2 \right) = 0
$$

avec $h(x,t)$ la hauteur d'eau et $q(x,t)$ le débit. La variable $g$ est la constante de gravité.

Le débit est lié à la hauteur d'eau par la relation :

$$
q(x,t) = h(x,t) u(x,t)
$$

avec $u(x,t)$ la vitesse de l'eau.

## Discrétisation

On considère ici un canal 1D de longueur $L$ avec des conditions aux limites réfléchissantes (des conditions périodiques sont aussi disponibles).

On discrétise l'espace en $N$ points de discrétisation régulièrement espacés de $\Delta x$.

On a donc :

$$
\Delta x = \frac{L}{N-1}
$$

On note $h_i^n$ la hauteur d'eau au point $i$ à l'instant $n$ et $q_i^n$ le débit au point $i$ à l'instant $n$.

On note $u_i^n$ la vitesse de l'eau au point $i$ à l'instant $n$.

On note $t_n$ l'instant $n$ et $x_i$ la position $i$.

On résout alors les équations en eau peu profonde en 1D en utilisant un schéma de type Leap-Frog.
Ce schéma n'est pas le meilleur mais il a l'avantage d'être simple à implémenter.

## Description du projet

Le projet est composé de plusieurs dossiers :

- `consignes` :  contient les consignes du projet. Elles sont organisées en plusieurs parties chacune dans un fichier `md` différent.
- `python/sequential` : contient une implémentation séquentielle de l'équation en eau peu profonde en 1D en Python.
- `cpp/kokkos` : contient une implémentation C++ kokkos de l'équation en eau peu profonde en 1D. Cette implémentation ne sera pas utilisée pour ce projet.
- `visualization` : contient des scripts Python pour visualiser les résultats de l'équation en eau peu profonde en 1D.

## Description du code séquentiel

## Visualisation des résultats

## Consignes








