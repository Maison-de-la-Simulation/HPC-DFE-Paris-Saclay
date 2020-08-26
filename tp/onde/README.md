# Cas de l'équation d'onde par la méthode des différences finies

Ce cas applicatif s'intéresse à la parallèlisation d'un code résolvant
l'équation d'onde 2D avec la méthode des différences finies.

L'équation d'onde que nous souhaitons résoudre correspond à l'équation dans un milieu homogène et isotrope :
<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//93c102b192a379c9a1541a17339633ee.svg?invert_in_darkmode" align=middle width=197.04135pt height=35.777445pt/></p>
Où <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//3e18a4a28fdee1744e5e3f79d13b9ff6.svg?invert_in_darkmode" align=middle width=7.113876000000004pt height=14.155350000000013pt/> est la vitesse de propagation des ondes dans le milieu dans toutes les directions (isotropie).
Le terme <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode" align=middle width=9.817500000000004pt height=22.831379999999992pt/> est ici un terme source.

En 2D, cette équation devient :
```math
\frac{\partial^2 U(\mathbf{x},t)}{\partial t^2} = c^2 \cdot \left[ \frac{\partial^2 U(\mathbf{x},t)}{\partial x^2}  + \frac{\partial^2 U(\mathbf{x},t)}{\partial y^2}\right] + f
```

Dans cette exemple, nous utilisons des conditions limites de type Dirichlet.
```math
U(\mathbf{x},t)_{boundary} = 0
```
Cela correspond à une réflection parfaite de l'onde sur les bords du domaine.
On peut noter qu'une condition de type Neumann nulle correspondrait à une absorption parfaite.

La discrétisation par la méthode des différences finies donne:
```math
U^{n+1}_{i,j} = 2 U^{n}_{i,j} - U^{n-1}_{i,j} + D  \cdot \left( U^{n}_{i+1,j} + U^{n}_{i-1,j} - 4 U^{n}_{i,j} + U^{n}_{i,j+1} + U^{n}_{i,j-1}  \right) + \Delta t ^2 f
```
où <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//c81c4840e92800e7ff99e5e3cd60ed4e.svg?invert_in_darkmode" align=middle width=23.198670000000003pt height=22.831379999999992pt/> est un coefficient valant <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//6eacc39a814e2cc2610a6146ebc26297.svg?invert_in_darkmode" align=middle width=92.833125pt height=31.360889999999984pt/>

Ici, <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//f7d46579966c0776a4ee06b2a64b0234.svg?invert_in_darkmode" align=middle width=28.767255000000002pt height=22.831379999999992pt/> est le pas de temps. On a donc <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//713282bc8540effe774c90aff9bf7edf.svg?invert_in_darkmode" align=middle width=78.35982pt height=22.831379999999992pt/>. Il faut alors comprendre que pour obtenir le temps <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//0d2a747a7bdaf44ca8311123afc6d55c.svg?invert_in_darkmode" align=middle width=47.30979pt height=22.831379999999992pt/>
nous avons besoin des deux pas de temps précédent.
<img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//c787a616fbd76e9c6c23bb63a746d835.svg?invert_in_darkmode" align=middle width=32.226150000000004pt height=22.831379999999992pt/> représentent le pas d'espace à la fois dans les directions <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//be12a978e6d1bafbac7cb59d0d63d3ba.svg?invert_in_darkmode" align=middle width=18.527520000000003pt height=22.831379999999992pt/> et <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//c7dea865aff30fe079cee413fde80793.svg?invert_in_darkmode" align=middle width=17.781720000000004pt height=22.831379999999992pt/>.
Les indices <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//db08780ad2ba9746939e86b175ce5161.svg?invert_in_darkmode" align=middle width=14.795715000000003pt height=22.831379999999992pt/> et <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//8bfa4e773baaafee75f9b9e78a22a43a.svg?invert_in_darkmode" align=middle width=16.84287pt height=22.831379999999992pt/> correspondent aux positions suivant <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//be12a978e6d1bafbac7cb59d0d63d3ba.svg?invert_in_darkmode" align=middle width=18.527520000000003pt height=22.831379999999992pt/> et <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//c7dea865aff30fe079cee413fde80793.svg?invert_in_darkmode" align=middle width=17.781720000000004pt height=22.831379999999992pt/>
sur la grille 2D de telle sorte que <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//56cf14a9459efcebf4fe3e20aa316ea2.svg?invert_in_darkmode" align=middle width=81.07407pt height=22.831379999999992pt/> et <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//ed47dc1eaf7c938c0695701dc69b6046.svg?invert_in_darkmode" align=middle width=82.37542499999999pt height=22.831379999999992pt/>.
De la même façon, <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//f7d46579966c0776a4ee06b2a64b0234.svg?invert_in_darkmode" align=middle width=28.767255000000002pt height=22.831379999999992pt/> est le pas de temps de telle sorte qu'en
définissant l'itération <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//fbdb696db6a1b0322aa20999d63696f2.svg?invert_in_darkmode" align=middle width=18.207915000000003pt height=22.831379999999992pt/>, nous obtenons <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//d26936ddd4b2a81ce158189288fb89ce.svg?invert_in_darkmode" align=middle width=77.56831500000001pt height=22.831379999999992pt/>.

La condition de stabilité du schéma est donnée par la formule :
```math
dt \leq dx / c
```

Les implémentations présentées dans ce dossier respectent ce formalisme.

## Implémentations

L'équation 2D est implémentée dans le code séquentiel fortran situé dans le dossier [applications/onde/fortran](./fortran/).
Les instructions de compilation et d'éxecution du code sont disponibles sur ce [README](./fortran/README.md).

## Analyse des résultats

L'éxecution du code genère des fichiers de sortie binaires dans un dossier `diags`
qui sera créé au début de chaque *run*.
Les fichiers de sortie portent le nom suivant `grid_<numéro d'itération>.dat`.
Le dossier [applications/onde/python](./python/`) contient des scripts Python
dédiés à l'analyse des résultats.
Les instructions pour utiliser ces scripts sont disponibles sur le [README](./python/README.md) correspondant.
