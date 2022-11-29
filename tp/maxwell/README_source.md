# Simulation des équations de Maxwell via la méthode aux différences finis explicite centrée de Yee

## Présentation du modèle

Nous nous intéressons dans ce projet à la simulation des équations Maxwell par l'utilisation d'une méthode aux différences finies très connue du nom de FDTD pour Finite-Difference Time-Domain publié par K. Yee dans les années 60 et toujours utilisée aujourd'hui dans les codes de calcul pour sa rapidité et sa simplicité.

On rappelle les équations de Maxwell dans le vide :

Maxwell Ampere :

$$
\frac{1}{c^2} \frac{\partial E}{\partial t}  = - \mu_0 \vec{J} + \nabla \times \vec{B}
$$

Maxwell Faraday :

$$
\frac{\partial B}{\partial t} = - \nabla \times \vec{E}
$$

Maxwell Gauss :

$$
\nabla \cdot \vec{E} = 0
$$

Maxwell Flux :

$$
\nabla \cdot \vec{B} = 0
$$

Dans le cadre de la discrétisation de Yee, on ne s'intéresse qu'à Maxwell Ampere et Faraday. On commence par normaliser les équations pour éliminer les constantes et réduire l'amplitude des champs (ce qui est toujours préférable en informatique).

$$
\frac{\partial E}{\partial t} = - \vec{J} + \nabla \times vec{B}
\frac{\partial B}{\partial t} = - \nabla \times vec{E}
$$

En 2D, la discrétisation donne les équations suivantes :

$$
Ex_{i,j}^{n+1} = Ex_{i,j}^{n} + \frac{\Delta t}{\Delta x} \left( Bz_{i,j+1}^{n+1/2}  - Bz_{i,j}^{n+1/2}\right)
$$

Où $n = t / \Delta t $ est l'iteration en temps, $i$ et $j$ respectivement la discrétisation en espace suivant les directions x et y.

<img src="../../support/materiel/marble.png" height="400">
