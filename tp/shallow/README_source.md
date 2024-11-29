# Equation en eau peu prodonde ou équation de Saint-Venant

Pour en apprendre plus :
- [Excellent cours de P.-Y. Lagrée](http://www.lmm.jussieu.fr/~lagree/COURS/MFEnv/code_C_saintvenant.pdf)
- [Shallow Water Equations](https://en.wikipedia.org/wiki/Shallow_water_equations)

## Introduction

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

<img src="../../support/materiel/shallow_parameters.svg" height="400">

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

On a alors :

$$
h_i^{n+1} = h_i^{n} - \frac{\Delta t}{\Delta x} \left( q_{i+1/2}^{n+1/2} - q_{i-1/2}^{n+1/2} \right)
$$

et 

$$
q_i^{n+1} = q_i^{n} - \frac{\Delta t}{\Delta x} \left( \frac{q_{i+1/2}^{n+1/2}^2}{h_{i+1/2}^{n+1/2}} + \frac{1}{2} g h_{i+1/2}^{n+1/2} - \frac{q_{i-1/2}^{n+1/2}^2}{h_{i-1/2}^{n+1/2}} - \frac{1}{2} g h_{i-1/2}^{n+1/2} \right)
$$

Les valeurs de $q$ et $h$ au au temps intermédiaire $n+1/2$ sont calculées en utilisant les valeurs de $q$ et $h$ au temps $n$.

$$
h_{i+1/2}^{n+1/2} = \frac{h_{i+1}^{n} + h_{i}^{n}}{2} - \frac{\Delta t}{2 \Delta x} \left( q_{i+1}^{n} - q_{i}^{n} \right)
$$

et

$$
q_{i+1/2}^{n+1/2} = \frac{q_{i+1}^{n} + q_{i}^{n}}{2} - \frac{\Delta t}{2 \Delta x} \left( \frac{q_{i+1}^{n}^2}{h_{i+1}^{n}} + \frac{1}{2} g h_{i+1}^{n} - \frac{q_{i}^{n}^2}{h_{i}^{n}} - \frac{1}{2} g h_{i}^{n} \right)
$$

Le schéma ci-dessous vous illustre la discrétisation en temps et en espace :

<img src="../../support/materiel/shallow_discretization.svg" height="400">

## Objectifs du projet

L'objectif de ce projet est de résoudre en parallèle grâce à MPI l'équation en eau peu profonde conservatrice en 1D.
Pour cela vous aurez un code séquentiel en Python qui vous servira de base de travail.
En suivant les consignes pas à pas, vous devrez paralléliser ce code séquentiel en utilisant MPI.

Pour le paralléliser, vous devrez découper le domaine en plusieurs sous-domaines de taille égale et chaque processus MPI devra s'occuper d'un sous-domaine. On appelle cela la décomposition de domaine. C'est une méthode très classique pour distribuer un problème en parallèle. Cela revient à résoudre sur chaque domaine la même équation en eau peu profonde mais sur un sous-domaine plus petit. On peut voir ça comme un ensemble de plusieurs simulations qui devront communiquer entre elles pour échanger les informations aux frontières des sous-domaines.

Commençons par faire un découpage en points : on distribue les points sur chaque processus MPI. On obtient donc un découpage comme ceci si on considère 3 processus MPI : 

<img src="../../support/materiel/shallow_domain_decomposition.svg" height="400">

En l'état, on remarque qu'un problème se pose pour le calcul des points situés aux bords des sous-domaines MPI (en vert sur le schéma). Il est nécessaire pour cela d'ajouter ce que l'on appelle des cellules fantômes. 
Ce sont des cellules supplémentaires qui étendent le domaine de calcul de chaque processus MPI et qui recouvre les cellules des processus voisins.
Avant d'effectuer un calcul au sein du sous-domaine, chaque processus MPI doit donc récupérer les valeurs des cellules fantômes des processus voisins afin d'avoir les dernières données à jour.

<img src="../../support/materiel/shallow_domain_decomposition_2.svg" height="400">

En ajoutant les cellules fantômes, on commence par mettre à jour les points aux bords (les tirets verts et oranges) à partir des données sur les voisins.

Cela permet de calculer les centres de cellules (les ronds verts et rouges) avant de mettre à jour les points intérieurs (tirets noirs).

En pratique, on effectuera les échanges après le calcul des points intérieurs.

Il sera nécessaire de calculer les index de début de chaque sous-domaine ``local_start``. Ces index permettent de définir les limites de chaque sous-domaine et de savoir quelles cellules sont des cellules fantômes et quelles cellules sont des cellules intérieures.

<img src="../../support/materiel/shallow_domain_decomposition_3.svg" height="400">

## Description du projet

Le projet est composé de plusieurs dossiers :

- `consignes` :  contient les consignes du projet. Elles sont organisées en plusieurs parties chacune dans un fichier `md` différent.
- `python/sequential` : contient une implémentation séquentielle de l'équation en eau peu profonde en 1D en Python.
- `visualization` : contient des scripts Python pour visualiser les résultats de l'équation en eau peu profonde en 1D.
- `cpp/kokkos` : contient une implémentation C++ kokkos de l'équation en eau peu profonde en 1D. Cette implémentation ne sera pas utilisée pour ce projet.

## Description du code séquentiel

Le code séquentiel est divisé en plusieurs parties : 

* `Input parameters` : définition des paramètres d'entrée
* `Initialization` : initialisation des variables et des conditions initiales
* `Terminal output summary` : affichage des informations de sortie dans le terminal
* `Time loop` : boucle en temps
* `Timers` : mesure du temps

Les paramètres d'entrée sont les suivants :

* `L` : La taille du domaine (en ligne de commande : `-L`)
* `N` : Le nombre de points de discrétisation (en ligne de commande : `-N`)
* `iterations` : Le nombre d'itérations (en ligne de commande : `-i`)
* `print_period` : Le nombre d'itérations entre chaque affichage dans le terminal (en ligne de commande : `-p`)
* `g` : La constante de gravité
* `output_period` : Le nombre d'itérations entre chaque sortie sur le disque (en ligne de commande : `-o`)
* `matplotlib_period` : Le nombre d'itérations entre chaque affichage avec matplotlib (en ligne de commande : `-mp`)
* `matplotlib_pause_duration` : Le temps d'affichage de chaque figure matplotlib (en ligne de commande : `-md`)

Il est possible de faire un affichage en direct de la simulation avec matplotlib en utilisant la variable `matplotlib_period` et `matplotlib_pause_duration`.
Cette option est pratique pour voir ce qu'il se passe dans la simulation mais elle ralentit considérablement le code.

Les paramètres peuvent aussi être donnés en ligne de commande. Par exemple :

```
python shallow_water.py -L 1000 -N 1000 -i 1000 -p 100 -o 100 -mp 100 -md 0.01
```

## Visualisation des résultats

Une fois la simulation terminée, vous pouvez visualiser les résultats écrits dans le dossier `diags`.

Dans le dossier `visualization`, vous trouverez 2 scripts python pour visualiser les données :

* `plot.py` : permet de visualiser un fichier précis
* `animate.py` : permet de visualiser une animation de la simulation

Par exemple pour visualiser le fichier `diags/diag_09900.bin` (on donne le chemin vers le fichier) :

```bash
python plot.py ./diags/diag_09900.bin
```

Ou pour animer tous les fichiers (on donne le chemin vers le dossier `diags`) :

```bash
python3 animate.py ./diags
```


## Consignes

Les consignes du cours sont disponibles dans le dossier `./consignes`. Elles sont organisées en plusieurs parties chacune dans un fichier `md` différent :

* `1_sequentiel.md` : découverte du code séquentiel
* `2_machines.md` : découverte de la machine parallèle
* `3_mpi.md` : parallélisation avec MPI
* `4_performance.md` : analyse de la performance
* `5_aide.md` : aide et conseils








