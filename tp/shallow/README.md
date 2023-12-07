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

<img src="../../support/materiel/shallow_water_discretization.svg" height="400">

## Description du projet

Le projet est composé de plusieurs dossiers :

- `consignes` :  contient les consignes du projet. Elles sont organisées en plusieurs parties chacune dans un fichier `md` différent.
- `python/sequential` : contient une implémentation séquentielle de l'équation en eau peu profonde en 1D en Python.
- `cpp/kokkos` : contient une implémentation C++ kokkos de l'équation en eau peu profonde en 1D. Cette implémentation ne sera pas utilisée pour ce projet.
- `visualization` : contient des scripts Python pour visualiser les résultats de l'équation en eau peu profonde en 1D.

## Description du code séquentiel

## Visualisation des résultats

## Consignes








