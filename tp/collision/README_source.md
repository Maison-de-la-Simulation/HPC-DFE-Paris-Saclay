# Simulation de particules par la mécanique du point avec collisions

## Présentation des modèles

### Mouvement

Ce cas applicatif simule un ensemble de particules (sphère) par les équations classiques de Newton en 3D.
La simulation intègre l'effet des frottements et de la gravité.
Elle intègre également un module de collisions.

L'équation du mouvement qui s'applique à chaque particule est la suivante :
$$
m \frac{d \vec{v}}{ dt} = m \vec{g} + \alpha \frac{\vec{v}}{\| v \|}
$$

Où $\vec{g}$ est le vecteur gravité, $m$ la masse de la particule, $\alpha$ un coefficient de freinage
et $\vec{v}$ le vecteur vitesse de la particule.

### Collisions

L'opérateur de collision modélise les particules comme des sphères ayant un rayon $R$ donné.
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

### Mouvement

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

### Collision

Cette partie décrit le fonctionnement de l'opérateur de collision.
On commencer par décrire quelques grandeurs.
On appelle $\overrightarrow{v_r}$ la vitesse relative entre deux particules :

$$
\overrightarrow{v_r} = \overrightarrow{v_1} - \overrightarrow{v_2}
$$

Cela revient à se placer dans le repère de la première particule.

Et $\overrightarrow{x_r}$ la direction relative entre les deux positions :

$$
\overrightarrow{x_r} = \| \overrightarrow{x_1} - \overrightarrow{x_2} \|
$$

Cela revient à placer la première particule au centre de notre repère.

La distance entre 2 particules est donnée par :

$$
D = \| \overrightarrow{x_r} \|
$$

On dit qu'il y a collision entre deux particules lorsque la distance entre les deux centres de masse est inférieur à deux rayons $D \leq 2R$
et lorsque les deux centres continuent de se rapprocher.
Cela revient à dire que dans la direction relative des centres de masse, la vitesse projetée est positive :

$$
\frac{ \overrightarrow{v_r} \cdot \overrightarrow{x_r} }{D} > 0
$$

Ces conditions imposent notre pas de temps $\Delta t$ pour prendre en compte toutes les collisions.
Entre chaque pas de temps, la distance relative que deux particules peuvent parcourir ne doit pas dépasser deux rayons $2R$.
Dans le cas contraire certaines particules se traversent sans se collisionner.

<img src="../../support/materiel/collision.png" height="200">

Lorsqu'il y a collision, la vitesse des particules est actualisé suivant la méthode mathématique présentée au dessus.
On appelle la vitesse post-collsion $v_n$.
Il faut ensuite actualiser les positions.
Etant donné que les particules se chevauchent au moment de la détection de la collision, il faut d'abord revenir en
arrière au moment où les particules rentrent en contact.

Pour cela, on utilise la vitesse avant collision pour déterminer le temps au moment de la collision $t_c$.
Soit $\Delta t_c$ le laps de temps entre le moment de la collision et le temps de simulation alors $t_c = t - \Delta t_c$.
Le but est de déterminer ce famueux $\Delta t_c$ afin de connaître les positions au moment de la collision.
Le moment de la collision est le moment où les particles rentrent en contact soit $D = 2R$.

Cela revient à résoudre le système suivant :

$$
2R = \| \left( \overrightarrow{x_1} - \overrightarrow{v_1} \Delta t_c \right)  - \left( \overrightarrow{x_2} - \overrightarrow{v_2} \Delta t_c \right)  \|
$$

On obtient une équation de second ordre :

$$
4 R^2 = v_{r,x}^2 + v_{r,y}^2 + v_{r,z}^2 -2 \left( x_r v_{r,x} + y_r v_{r,y} + z_r v_{r,z} \right) \Delta t_c + \left( v_{r,x}^2  + v_{r,y}^2 + v_{r,z}^2 \right) \Delta t_c^2
$$

La solutions sont nécessairement réelles si les particules se rapprochent. La bonne solution est celle qui est positivie.

On calcule donc la position au moment de la collision en faisant simplement : $\overrightarrow{x_c} = \overrightarrow{x} - \Delta t_c \cdot \overrightarrow{v}$.
Il s'agit d'une approximation car on ne prend pas en compte la gravité et les frottements.
Une fois la position $\overrightarrow{x_c}$ connue, on calcule la position post-collision $\overrightarrow{x_n}$ grâce à la vitesse post-collision :

$$
\overrightarrow{x_n} = \overrightarrow{x_c} + \Delta t_c * \overrightarrow{v_n}
$$

L'opérateur suppose que chaque particule ne collisionne qu'une fois avec une autre particule.
Pour cela, on utilise une algorithme à double boucles imbriquées où la seconde boucle démarre à partir de la particule $i+1$.
Soit $N$ le nombre total de particules.
```
Pour chaque particle i de 1 jusqu'à N :

    Si la particule i n'a pas déjà fait l'objet d'une collision :

        Pour chaque particule j de i+1 jusqu'à N :
            
            Si la particule j n'a pas déjà fait l'objet d'une collision :
            
                Si collision entre la particule i et j validée :
                
                    Actualisation des propriétés
                    
                    Les particules i et j sont marquées comme ayant subi une collision
```

On applique ensuite ce même opérateur plusieurs fois pour simuler les collisions multiples.

Il est possible de simuler des collisions inélastiques (perte d'énergie induite lors de la collision) en multipliant la nouvelle vitesse par un coefficient d'amortissement $\alpha$ de telle sorte que la nouvelle énergie de la particule vaut $0.5 \left( 1 - \alpha\right) m v^2$.

### Conditions aux bords

Les bords sont des murs réflécissants avec possibilité d'amortissement.
On définit une collision avec un mur au moment où la distance entre le mur et le centre de la particule est inférieur à $2R$.
Un mur est défini par une normal \overrightarrow{n} et un point \overrightarrow{p}. Par convention, la normale est orientée vers l'intéreur du domaine pour le calcul de la réflexion.
Comme pour les collisions entre particules, on calcule le temps au moment exact de la collision, c'est à dire quand la distance entre le mur et la particule vaut exactement $2R$.
Cela permet de calculer la position de la particule au moment de la collision avant d'actualiser sa vitesse et de calculer la position post-réflexion.

La vitesse post-réflexion $\overrightarrow{v_n}$ se calcule facilement par réflexion spéculaire :
$$
\overrightarrow{v_n} = \overrightarrow{v} - 2 \left( \overrightarrow{v} \cdot \overrightarrow{n} \right) \overrightarrow{n}
$$


## Description du répertoire

Ce TP se compose de plusieurs répertoires :
- [cpp](./cpp/) : ce dossier contient l'ensemble des sources du code séquentiel.
- [python](./python) : ce dossier contient des scripts dédiés à la visualization et à la comparaison des résultats (fichiers de sortie)
- [.extra](./.extra) : ce dossier sert uniquement pour GitHub

## Le code séquentiel

### Description

Le code est écrit en C++. C'est un C++ simple n'utilisant pas de capacité avancée du C++.
La version séquentielle se compose des fichhiers suivant :
- [main.cpp](./cpp/main.cpp)

### Les dépendances

Ce programme nécessite l'installation d'un compilateur C++.

Pour le TP, vous aurez besoin d'installer un compilateur équipé d'OpenMP.
C'est normalement le cas de la plupart des compilateurs récents.
C'est le cas sur les postes de travail de l'université.

Vous aurez aussi besoin d'installer MPI.
Sur les postes de travail de l'université, MPI est déjà installé.
Pour l'installer sur vos ordinateurs personnels, utilisez les instructions dans le dossier [./documentation](../../documentation/mpi.md).

Pour les sorties, vous aurez besoin d'HDF5.
Sur les ordinateurs de l'université, une version parallèle est installée dans l'espace commun : `/public/m2_dfe/hdf5-1.10.4/build`.
Pour l'installer sur vos ordinateurs personnels, utilisez les instructions dans le dossier [./documentation](../../documentation/hdf5.md).
Si vraiment HDF5 vous pose problème il est possible de désactiver cette sortie dans le code.

### Compilation et exécution

Pour compiler ce programme, vous pouvez tout simplement utiliser le makefile dans le dossier des sources en tapant dans votre terminal :
```bash
make
```

La compilation génère un fichier exécutable du nom de `executable`. Vous pouvez lancer le programme en faisant :
```bash
./executable
```

### Visualization

Pour visualiser les résultats, vous pouvez utiliser python. Pour cela, utilisez les scripts disponible dans le dossier [python](./python).
Vous avez besoin de python avec la biblithèque `matplotlib` et `h5py`.

Le code génère aussi des fichiers au format VTK.
Les fichiers sont créés indépendament de la bilbiothèque VTK.
Ces fichiers peuvent être visualisés à l'aide des logiciels VisIt ou Paraview.

## Consignes de TP
