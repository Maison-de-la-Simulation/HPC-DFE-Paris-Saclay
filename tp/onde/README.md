# Cas de l'équation d'onde par la méthode des différences finies

Ce cas applicatif s'intéresse à la parallélisation d'un code résolvant
l'équation d'onde 2D avec la méthode des différences finies.

L'équation d'onde que nous souhaitons résoudre correspond à l'équation dans un milieu homogène et isotrope :
<p align="center"><img src=".extra//93c102b192a379c9a1541a17339633ee.svg?invert_in_darkmode" align=middle width=197.04135pt height=35.777445pt/></p>

Où <img src=".extra//3e18a4a28fdee1744e5e3f79d13b9ff6.svg?invert_in_darkmode" align=middle width=7.113876000000004pt height=14.155350000000013pt/> est la vitesse de propagation des ondes dans le milieu dans toutes les directions (isotropie).
Le terme <img src=".extra//190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode" align=middle width=9.817500000000004pt height=22.831379999999992pt/> est ici un terme source.

En 2D, cette équation devient :
<p align="center"><img src=".extra//f231920c6ee6e0dc13ded86340835292.svg?invert_in_darkmode" align=middle width=323.598pt height=40.118265pt/></p>

Dans cette exemple, nous utilisons des conditions limites de type Dirichlet.

<p align="center"><img src=".extra//9c51fbaac26ac4e98105263a6ad85255.svg?invert_in_darkmode" align=middle width=135.65722499999998pt height=17.031959999999998pt/></p>

Cela correspond à une réflection parfaite de l'onde sur les bords du domaine.
On peut noter qu'une condition de type Neumann nulle correspondrait à une absorption parfaite.

La discrétisation par la méthode des différences finies donne:
<p align="center"><img src=".extra//8cb132f7fcd865f0f1637fbfe01228c2.svg?invert_in_darkmode" align=middle width=561.39765pt height=21.267014999999997pt/></p>

où <img src=".extra//78ec2b7008296ce0561cf83393cb746d.svg?invert_in_darkmode" align=middle width=14.066250000000002pt height=22.46574pt/> est un coefficient valant <img src=".extra//8eedfdb979b5e344bbc89b1b615bdcb0.svg?invert_in_darkmode" align=middle width=77.399355pt height=31.360889999999984pt/>

Ici, <img src=".extra//5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode" align=middle width=19.634835000000002pt height=22.46574pt/> est le pas de temps. On a donc <img src=".extra//920296fb97fdfa6eb814c39b79d4b30d.svg?invert_in_darkmode" align=middle width=69.2274pt height=22.46574pt/>. Il faut alors comprendre que pour obtenir le temps <img src=".extra//3f18d8f60c110e865571bba5ba67dcc6.svg?invert_in_darkmode" align=middle width=38.17737pt height=21.18732pt/>
nous avons besoin des deux pas de temps précédent.
<img src=".extra//3919bbc84b8079e27194efe99a1f6a80.svg?invert_in_darkmode" align=middle width=23.09373pt height=22.46574pt/> représentent le pas d'espace à la fois dans les directions <img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.395100000000005pt height=14.155350000000013pt/> et <img src=".extra//deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649300000000004pt height=14.155350000000013pt/>.
Les indices <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663295000000005pt height=21.683310000000006pt/> et <img src=".extra//36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710483000000004pt height=21.683310000000006pt/> correspondent aux positions suivant <img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.395100000000005pt height=14.155350000000013pt/> et <img src=".extra//deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649300000000004pt height=14.155350000000013pt/>
sur la grille 2D de telle sorte que <img src=".extra//688c52dc242b204f9355f962fa0a785d.svg?invert_in_darkmode" align=middle width=71.941485pt height=22.46574pt/> et <img src=".extra//f377d7434d3ed286e5468c02610c5446.svg?invert_in_darkmode" align=middle width=73.24300500000001pt height=22.46574pt/>.
De la même façon, <img src=".extra//5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode" align=middle width=19.634835000000002pt height=22.46574pt/> est le pas de temps de telle sorte qu'en
définissant l'itération <img src=".extra//63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode" align=middle width=9.075495000000004pt height=22.831379999999992pt/>, nous obtenons <img src=".extra//67b69ec298df7cfdfd3771e2719ba19b.svg?invert_in_darkmode" align=middle width=68.435895pt height=22.831379999999992pt/>.

La condition de stabilité du schéma est donnée par la formule :
<p align="center"><img src=".extra//14a85e84ef29e9208b124c8684811fc4.svg?invert_in_darkmode" align=middle width=69.69368999999999pt height=16.438356pt/></p>

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
