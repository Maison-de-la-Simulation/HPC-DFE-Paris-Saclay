# Simulation de particules par la mécanique du point avec collisions

## Présentation des modèles

Ce cas applicatif simule un ensemble de particules (sphère) par les équations classiques de Newton en 3D.
La simulation intègre l'effet des frottements et de la gravité.
Elle intègre également un module de collisions.

L'équation du mouvement qui s'applique à chaque particule est la suivante :
<p align="center"><img src=".extra//3ade19f5b4a6f3d812e3b551a00193ce.svg?invert_in_darkmode" align=middle width=137.90964pt height=38.250465pt/></p>

Où <img src=".extra//8b5bd5fd95868f24ad0a078d34768d7d.svg?invert_in_darkmode" align=middle width=10.747770000000003pt height=23.488740000000007pt/> est le vecteur gravité, <img src=".extra//0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode" align=middle width=14.433210000000003pt height=14.155350000000013pt/> la masse de la particule, <img src=".extra//c745b9b57c145ec5577b82542b2df546.svg?invert_in_darkmode" align=middle width=10.576500000000003pt height=14.155350000000013pt/> un coefficient de freinage
et <img src=".extra//cd74c822d31d457e590f28706c11499d.svg?invert_in_darkmode" align=middle width=10.747770000000004pt height=23.488740000000007pt/> le vecteur vitesse de la particule.

L'opérateur de collision modélise les particules comme des sphères ayant un rayon <img src=".extra//89f2e0d2d24bcf44db73aab8fc03252c.svg?invert_in_darkmode" align=middle width=7.873024500000003pt height=14.155350000000013pt/> donné.
Le modèle implémenté est inspiré des sources suivantes :
- https://www.plasmaphysics.org.uk/collision3d.htm
- https://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
- https://www.sjsu.edu/faculty/watkins/collision.htm
- https://studiofreya.com/3d-math-and-physics/simple-sphere-sphere-collision-detection-and-collision-response/
- https://sites.google.com/site/tipebillard/modelisation-des-frottements

Les collisions supposent que les moments et l'énergie sont conservés.
Soient deux particules entrant en collision, on utilise la notation <img src=".extra//59ee6605c4fda9aa685eda225e17ff16.svg?invert_in_darkmode" align=middle width=13.842840000000004pt height=14.155350000000013pt/> pour désigner la vitesse après la collision :
<p align="center"><img src=".extra//154240037fa246f8af778fa14332a038.svg?invert_in_darkmode" align=middle width=229.43579999999997pt height=14.292217499999998pt/></p>
Et :
<p align="center"><img src=".extra//de9c49ff21a6dae967b0f102760821bb.svg?invert_in_darkmode" align=middle width=314.63355pt height=20.504055pt/></p>

On appelle <img src=".extra//19e3f7018228f8a8c6559d0ea5500aa2.svg?invert_in_darkmode" align=middle width=10.747935000000007pt height=23.488740000000007pt/> le vecteur position d'une particule.
De fait, le changement de direction s'opère suivant la direction :
<p align="center"><img src=".extra//55c2dcdfde5f239ca9bc7eddbe44231d.svg?invert_in_darkmode" align=middle width=103.87013999999999pt height=38.250465pt/></p>
Et la modification sur le moment de la particule peut s'écrire :
<p align="center"><img src=".extra//4499dc78d88246247aa45a5c1779b2ec.svg?invert_in_darkmode" align=middle width=270.996pt height=20.78538pt/></p>

On en déduit ainsi la valeur de <img src=".extra//44bc9d542a92714cac84e01cbbb7fd61.svg?invert_in_darkmode" align=middle width=8.689230000000004pt height=14.155350000000013pt/> :

<p align="center"><img src=".extra//1c50131d46cc28b91a36004206e67eac.svg?invert_in_darkmode" align=middle width=193.4427pt height=30.84411pt/></p>
Et par ricochet les nouvelles vitesses après collision.

## Discrétisation et algorithmes

On utilise <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663295000000005pt height=21.683310000000006pt/> pour désigner la i-ème particule dans notre domaine.
On appelle <img src=".extra//4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode" align=middle width=5.936155500000004pt height=20.222069999999988pt/> le temps courant et <img src=".extra//5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode" align=middle width=19.634835000000002pt height=22.46574pt/> le pas en temps.
La discrétisation des équations du mouvement se fait par une méthode explicite classique du type *leap-frog*.
On décompose le vecteur vitesse suivant ses 3 composantes <img src=".extra//f3e82fa4e8995954825701a7c80f8494.svg?invert_in_darkmode" align=middle width=92.55988500000001pt height=23.488740000000007pt/>.

<p align="center"><img src=".extra//48d97fbc4aa2ef5b8bc4e23d7870ff76.svg?invert_in_darkmode" align=middle width=356.1855pt height=49.31553pt/></p>

De même, on décompose le vecteur position suivant ses 3 composantes <img src=".extra//fa3d60ab15e49b46f5a9c287ac69cf93.svg?invert_in_darkmode" align=middle width=72.555945pt height=23.488740000000007pt/>.

<p align="center"><img src=".extra//a120f83e9e76184b2030e4cf29e1fe9c.svg?invert_in_darkmode" align=middle width=169.3362pt height=19.477095pt/></p>