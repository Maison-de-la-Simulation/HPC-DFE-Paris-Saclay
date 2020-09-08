# Simulation de particules par la mécanique du point avec collisions

## Présentation des modèles

Ce cas applicatif simule un ensemble de particules (sphère) par les équations classiques de Newton en 3D.
La simulation intègre l'effet des frottements et de la gravité.
Elle intègre également un module de collisions.

L'équation du mouvement qui s'applique à chaque particule est la suivante :
$$
m \frac{d \vec{v}}{ dt} = m \vec{g} + \alpha \frac{\vec{v}}{\| v \|}
$$

Où $\vec{g}$ est le vecteur gravité, $m$ la masse de la particule, $\alpha$ un coefficient de freinage
et $\vec{v}$ le vecteur vitesse de la particule.

L'opérateur de collision modélise les particules comme des sphères ayant un rayon $r$ donné.
Le modèle implémenté est inspiré des sources suivantes :
- https://www.plasmaphysics.org.uk/collision3d.htm
- https://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
- https://www.sjsu.edu/faculty/watkins/collision.htm
- https://studiofreya.com/3d-math-and-physics/simple-sphere-sphere-collision-detection-and-collision-response/
- https://sites.google.com/site/tipebillard/modelisation-des-frottements

Les collisions supposent que les moments et l'énergie sont conservés.
Soient deux particules entrant en collision, on utilise la notation $v_c$ pour désigner la vitesse après la collision :
$$
m_1 v_1 + m_2 v_2 = m_1 v_{c,1} + m_2 v_{c,2}
$$
Et :
$$
0.5 m_1 v_1^2 + 0.5 m_2 v_2^2 = 0.5 m_1 v_{c,1}^2 + 0.5 m_2 v_{c,2}^2
$$

On appelle $\vec{x}$ le vecteur position d'une particule.
De fait, le changement de direction s'opère suivant la direction :
$$
\vec{k} = \frac{\vec{x_1} - \vec{x_2} }{ \| \vec{x_1} - \vec{x_2} \|}
$$
Et la modification sur le moment de la particule peut s'écrire :
$$
m_1 \left( v_{c,1} - v_1 \right)  = - m_2 \left( v_{c,2} - v_2 \right) = a \vec{k}
$$

On en déduit ainsi la valeur de $a$ :

$$
a = 2  k \cdot \left( v_{1} - v_{2} \right) \frac{m_1 m_2}{m1 + m2}
$$
Et par ricochet les nouvelles vitesses après collision.

## Discrétisation et algorithmes

On utilise $i$ pour désigner la i-ème particule dans notre domaine.
On appelle $t$ le temps courant et $\Delta t$ le pas en temps.
La discrétisation des équations du mouvement se fait par une méthode explicite classique du type *leap-frog*.
On décompose le vecteur vitesse suivant ses 3 composantes $\vec{v} = \left\{v_x, v_y, v_z\right\}$.

$$
\overrightarrow{v_{i}^{t + 0.5 \Delta t}} = \vec{v}_{i}^{t - 0.5 \Delta t} + \Delta t  \cdot \left( m \vec{g} + \alpha \frac{\vec{v}_i^{t - 0.5 \Delta t}}{\| \vec{v}_i^{t - 0.5 \Delta t} \|} \right)
$$

De même, on décompose le vecteur position suivant ses 3 composantes $\vec{x} = \left\{x, y, z \right\}$.

$$
\vec{x}_{i}^{t + \Delta t} = \vec{x}_{i}^{t} + \Delta t  \cdot \vec{v}_{i}^{t + \Delta t}
$$
