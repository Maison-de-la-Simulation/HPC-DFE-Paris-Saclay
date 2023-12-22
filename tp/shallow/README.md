# Equation en eau peu prodonde ou équation de Saint-Venant

Pour en apprendre plus :
- [Excellent cours de P.-Y. Lagrée](http://www.lmm.jussieu.fr/~lagree/COURS/MFEnv/code_C_saintvenant.pdf)
- [Shallow Water Equations](https://en.wikipedia.org/wiki/Shallow_water_equations)

## Objectifs

L'objectif de ce projet est de résoudre en parallèle grâce à MPI l'équation en eau peu profonde conservatrice en 1D.

## Principes

Les équations en eau peu profonde sont des équations hyperboliques non linéaires qui approximent l'évolution de la hauteur d'eau d'un canal, d'une retenue, des marées, etc. 

On s'intéresse par simplicité ici à la forme conservative de l'équation en eau peu profonde en 1D.

<img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> est l'axe spatial et <img src=".extra//4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode" align=middle width=5.936097749999991pt height=20.221802699999984pt/> le temps.

On a alors :

<p align="center"><img src=".extra//da8b741226d5b438916942d8c424e5b8.svg?invert_in_darkmode" align=middle width=94.29268859999999pt height=33.81208709999999pt/></p>

<p align="center"><img src=".extra//ef31e6d98a2f7af7791da83dde005918.svg?invert_in_darkmode" align=middle width=196.4695524pt height=40.11819404999999pt/></p>

avec <img src=".extra//a27a154b65070f4fd50a9569305d300e.svg?invert_in_darkmode" align=middle width=44.89351679999999pt height=24.65753399999998pt/> la hauteur d'eau et <img src=".extra//1c1234bab9a0971ca47482c25920fffe.svg?invert_in_darkmode" align=middle width=43.35048959999999pt height=24.65753399999998pt/> le débit. La variable <img src=".extra//3cf4fbd05970446973fc3d9fa3fe3c41.svg?invert_in_darkmode" align=middle width=8.430376349999989pt height=14.15524440000002pt/> est la constante de gravité.

Le débit est lié à la hauteur d'eau par la relation :

<p align="center"><img src=".extra//eb771483475c6b6c5fb210a0c09d672c.svg?invert_in_darkmode" align=middle width=154.9943109pt height=16.438356pt/></p>

avec <img src=".extra//9a1205e73049dcbe49e500982405ce76.svg?invert_in_darkmode" align=middle width=44.832674699999984pt height=24.65753399999998pt/> la vitesse de l'eau.

## Discrétisation

On considère ici un canal 1D de longueur <img src=".extra//ddcb483302ed36a59286424aa5e0be17.svg?invert_in_darkmode" align=middle width=11.18724254999999pt height=22.465723500000017pt/> avec des conditions aux limites réfléchissantes (des conditions périodiques sont aussi disponibles).

<img src="../../support/materiel/shallow_parameters.svg" height="400">

On discrétise l'espace en <img src=".extra//f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> points de discrétisation régulièrement espacés de <img src=".extra//3919bbc84b8079e27194efe99a1f6a80.svg?invert_in_darkmode" align=middle width=23.09366069999999pt height=22.465723500000017pt/>.

On a donc :

<p align="center"><img src=".extra//652c1294ead7baca4fb8aada7d2e42b6.svg?invert_in_darkmode" align=middle width=90.2942568pt height=34.999293449999996pt/></p>

On note <img src=".extra//3604591fc2536fef0796984cbe7a104a.svg?invert_in_darkmode" align=middle width=17.59713779999999pt height=22.831056599999986pt/> la hauteur d'eau au point <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> à l'instant <img src=".extra//55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/> et <img src=".extra//a1ba3f6d4e7498de97c6dde4ffeb491d.svg?invert_in_darkmode" align=middle width=16.05411059999999pt height=21.839370299999988pt/> le débit au point <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> à l'instant <img src=".extra//55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/>.

On note <img src=".extra//6302bc0298881fdd4cf1954733ad3f9a.svg?invert_in_darkmode" align=middle width=17.53629569999999pt height=21.839370299999988pt/> la vitesse de l'eau au point <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> à l'instant <img src=".extra//55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/>.

On note <img src=".extra//27413cd33c6f718117d8fb364284f787.svg?invert_in_darkmode" align=middle width=14.06212004999999pt height=20.221802699999984pt/> l'instant <img src=".extra//55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/> et <img src=".extra//9fc20fb1d3825674c6a279cb0d5ca636.svg?invert_in_darkmode" align=middle width=14.045887349999989pt height=14.15524440000002pt/> la position <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/>.

On résout alors les équations en eau peu profonde en 1D en utilisant un schéma de type Leap-Frog.
Ce schéma n'est pas le meilleur mais il a l'avantage d'être simple à implémenter.

On a alors :

<p align="center"><img src=".extra//02b0ee35d24218cb4db3b50b2281bfb1.svg?invert_in_darkmode" align=middle width=258.61056705pt height=33.62942055pt/></p>

et 

<p align="center"><img src=".extra//3bd3a7a05c89a5da6db5ad63c607dc3d.svg?invert_in_darkmode" align=middle width=465.36947489999994pt height=59.1786591pt/></p>

Les valeurs de <img src=".extra//d5c18a8ca1894fd3a7d25f242cbe8890.svg?invert_in_darkmode" align=middle width=7.928106449999989pt height=14.15524440000002pt/> et <img src=".extra//2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode" align=middle width=9.47111549999999pt height=22.831056599999986pt/> au au temps intermédiaire <img src=".extra//e2786a754952ac6992cd3299d8ccb6a2.svg?invert_in_darkmode" align=middle width=54.61569629999999pt height=24.65753399999998pt/> sont calculées en utilisant les valeurs de <img src=".extra//d5c18a8ca1894fd3a7d25f242cbe8890.svg?invert_in_darkmode" align=middle width=7.928106449999989pt height=14.15524440000002pt/> et <img src=".extra//2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode" align=middle width=9.47111549999999pt height=22.831056599999986pt/> au temps <img src=".extra//55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/>.

<p align="center"><img src=".extra//44137b708da8839b777c8ca64ff621c0.svg?invert_in_darkmode" align=middle width=283.8912186pt height=34.750915649999996pt/></p>

et

<p align="center"><img src=".extra//ee822ec16cfd1ae6da776916e452a097.svg?invert_in_darkmode" align=middle width=441.8228446499999pt height=42.668194799999995pt/></p>

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








