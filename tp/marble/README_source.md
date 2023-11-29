# Simulation de particules par la mécanique du point avec collisions

<img src="../../support/materiel/marble.png" height="400">

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
L'équation du mouvement discrétisée devient :

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

On dit qu'il y a collision entre deux particules lorsque la distance entre les deux centres de masse est inférieure à deux rayons $D \leq 2R$
et lorsque les deux centres continuent de se rapprocher.
Cela revient à dire que dans la direction relative des centres de masse, la vitesse projetée est positive :

$$
\frac{ \overrightarrow{v_r} \cdot \overrightarrow{x_r} }{D} > 0
$$

Ces conditions imposent notre pas de temps $\Delta t$ pour prendre en compte toutes les collisions.
Entre chaque pas de temps, la distance relative que deux particules peuvent parcourir ne doit pas dépasser deux rayons $2R$.
Dans le cas contraire certaines particules se traversent sans se collisionner.

<img src="../../support/materiel/collision.png" height="200">

Lorsqu'il y a collision, la vitesse des particules est actualisée suivant la méthode mathématique présentée au-dessus.
On appelle la vitesse post-collision $v_n$.
Il faut ensuite actualiser les positions.
Etant donné que les particules se chevauchent au moment de la détection de la collision, il faut d'abord revenir en
arrière au moment où les particules rentrent en contact.

Pour cela, on utilise la vitesse avant collision pour déterminer le temps au moment de la collision $t_c$.
Soit $\Delta t_c$ le laps de temps entre le moment de la collision et le temps de simulation alors $t_c = t - \Delta t_c$.
Le but est de déterminer ce fameux $\Delta t_c$ afin de connaître les positions au moment de la collision.
Le moment de la collision est le moment où les particules rentrent en contact soit $D = 2R$.

Cela revient à résoudre le système suivant :

$$
2R = \| \left( \overrightarrow{x_1} - \overrightarrow{v_1} \Delta t_c \right)  - \left( \overrightarrow{x_2} - \overrightarrow{v_2} \Delta t_c \right)  \|
$$

On obtient une équation de second ordre :

$$
4 R^2 = v_{r,x}^2 + v_{r,y}^2 + v_{r,z}^2 -2 \left( x_r v_{r,x} + y_r v_{r,y} + z_r v_{r,z} \right) \Delta t_c + \left( v_{r,x}^2  + v_{r,y}^2 + v_{r,z}^2 \right) \Delta t_c^2
$$

Les solutions sont nécessairement réelles si les particules se rapprochent. La bonne solution est celle qui est positive.

On calcule donc la position au moment de la collision en faisant simplement : $\overrightarrow{x_c} = \overrightarrow{x} - \Delta t_c \cdot \overrightarrow{v}$.
Il s'agit d'une approximation car on ne prend pas en compte la gravité et les frottements.
Une fois la position $\overrightarrow{x_c}$ connue, on calcule la position post-collision $\overrightarrow{x_n}$ grâce à la vitesse post-collision :

$$
\overrightarrow{x_n} = \overrightarrow{x_c} + \Delta t_c \cdot \overrightarrow{v_n}
$$

L'opérateur suppose que chaque particule ne collisionne qu'une fois avec une autre particule.
Pour cela, on utilise un algorithme à double boucles imbriquées où la seconde boucle démarre à partir de la particule $i+1$.
Soit $N$ le nombre total de particules.
```
Pour chaque particule i de 1 jusqu'à N :

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

Les bords sont des murs réfléchissants avec possibilité d'amortissement.
On définit une collision avec un mur au moment où la distance entre le mur et le centre de la particule est inférieur à $2R$.
Un mur est défini par une normal $\overrightarrow{n}$ et un point $\overrightarrow{p}$. Par convention, la normale est orientée vers l'intérieur du domaine pour le calcul de la réflexion.
Comme pour les collisions entre particules, on calcule le temps au moment exact de la collision, c'est à dire quand la distance entre le mur et la particule vaut exactement $2R$.
Cela permet de calculer la position de la particule au moment de la collision avant d'actualiser sa vitesse et de calculer la position post-réflexion.

La vitesse post-réflexion $\overrightarrow{v_n}$ se calcule facilement par réflexion spéculaire :
$$
\overrightarrow{v_n} = \overrightarrow{v} - 2 \left( \overrightarrow{v} \cdot \overrightarrow{n} \right) \overrightarrow{n}
$$


## Description du répertoire

Ce TP se compose de plusieurs répertoires :
- [cpp/sequential](./cpp/sequential) : ce dossier contient l'ensemble des sources du code séquentiel.
- [cpp/patch](./cpp/patch) : ce dossier contient l'ensemble des sources du code séquentiel avec décomposition en sous-domaine (*patch*).
- [cpp/mpi](./cpp/mpi) : ce dossier contient la version mpi du code.
- [cpp/mpi](./cpp/omp) : ce dossier contient la version OpenMP du code.
- [scalings](./python) : ce dossier contient des scripts permettant d'afficher les courbes de scalabilité présentées à la fin de ce document.
- [visualization](./python) : ce dossier contient des scripts dédiés à la visualisation.
- [consignes](./consignes) : ce dossier contient les consignes du TP
- [.extra](./.extra) : ce dossier sert uniquement pour GitHub

## Le code séquentiel

### Description

Le code est écrit en C++.
C'est un C++ simple n'utilisant pas de capacité avancée du C++.
Néanmoins, il est nécessaire d'être familier avec la notion de classe et de méthode.

Afin de préparer le terrain du parallélisme, le code séquentiel a été développé en utilisant un modèle de décomposition de domaine.
Le domaine de simulation est découpé en sous-domaine que l'on appelle *patch* comme le montre la figure ci-dessous.
Chaque *patch* possède ses particules. Lorsque les particules changent de *patch*, il est nécessaire de les communiquer aux *patches* qui les reçoivent.
Dans une version séquentielle normale, la décomposition en sous-domaine est inutile.
Toutes les particules sont traitées comme appartenant au même domaine.
Néanmoins, la décomposition de domaine sera nécessaire dans les versions parallèles.

<img src="../../support/materiel/patch.png" height="400">

La version séquentielle avec *patch* se compose des fichiers et headers suivant :
- [main.cpp](./cpp/patch/main.cpp)
- [parameters.cpp](./cpp/patch/parameters.cpp) et [parameters.h](./cpp/patch/parameters.h) : ce fichier contient la description de structure décrivant les propriétés du domaine et de la simulation ainsi qu'une fonction permettant de lire les arguments en ligne de commande
- [particles.cpp](./cpp/patch/particles.cpp) et [particles.h](./cpp/patch/particles.h) : ce fichier décrit la classe `Particles`. Cette classe représente l'ensemble des patchs du domaine et donc l'ensemble des particules.
- [patch.cpp](./cpp/patch/patch.cpp) et [patch.h](./cpp/patch/patch.h) : ce fichier décrit la classe `Patch`. Cette classe représente un *patch* du domaine de simulation et donc que les particules de cette sous-région.
- [walls.cpp](./cpp/patch/walls.cpp) et [walls.h](./cpp/patch/walls.h) : ce fichier décrit la classe `Walls`. Celle classe représente un mur sur lequel les particules rebondissent.
- [timers.cpp](./cpp/patch/timers.cpp) et [timers.h](./cpp/patch/timers.h) : ce fichier décrit la classe `Timer`. Cette classe est utilisée pour mesurer et gérer le temps passé dans chaque fonction du code.

Dans la version par *patch*, on fait l'approximation que les collisions ne s'appliquent qu'au sein des *patches*.
Cela signifie que l'on néglige les collisions qui devraient avoir lieu entre particules de *patches* différents proches des frontières respectives.
La prise en compte de ce faible nombre de collisions nécessiteraient une complexification du code et des processus d'échange entrainant alors une complexification inutile de ce TP.

### Les processus d'échange entre *patch*

La procédure d'échange fonctionne avec les étapes suivantes :
1. Chaque *patch* identifie les particules qui doivent être transférer vers un voisin car elles ne sont plus dans ses limites spatiales.
Les particules qui sortent du *patch* sont déplacées dans des *buffers*.
Les buffers sont simplement des tableaux de particules pour mise en attente avant transfert.
Il existe un buffer par directions.
En 3D par exemple chaque patch a 26 voisins.
De fait, il y a 26 directions en incluant les faces, les arrêtes et les coins.
Dans le tableau principal des particules (celles du *patch*), les particules sont simplement marquées afin de les supprimer de manière optimisée à l'étape suivante.

<img src="../../support/materiel/particle_exchange_process.png" height="400">

2. La deuxième étape est la suppression des particules ayant quittée le *patch* du tableau principale. Il est préférable d'effectuer cette opération pour toutes les particules en une fois car l'algorithme utilisé est plus efficace.
La méthode consiste à remplir les cases mémoires vides des particules ayant quittée le *patch* par les particules de la fin du tableau.
3. Dans la troisième étape, chaque *patch* va chercher dans les *buffers* de ses voisins les particules qu'ils doivent recevoir.

### Les dépendances

Ce programme nécessite l'installation d'un compilateur C++.

Pour le TP, vous aurez besoin d'installer un compilateur équipé d'OpenMP.
C'est normalement le cas de la plupart des compilateurs récents.
C'est le cas sur les postes de travail de l'université.

Vous aurez aussi besoin d'installer MPI.
Sur les postes de travail de l'université, MPI est déjà installé.
Pour l'installer sur vos ordinateurs personnels, utilisez les instructions dans le dossier [./documentation](../../documentation/mpi.md).

### Compilation et exécution

Pour compiler ce programme, vous pouvez tout simplement utiliser le makefile dans le dossier des sources en tapant dans votre terminal :
```bash
make
```

 Le makefile est là pour vous simplifier la vie.
 Il est possible aussi de compiler à la main mais il faut savoir le faire.
 Étant donné que le projet se compose de plusieurs fichiers il faut compiler chaque fichier séparément et ensuite faire ce qu'on appelle une étape de linking c'est à dire qu'on rassemble tous les fichiers temporaires compilés pour construire le binaire.

Si jamais vous avez des problèmes de compilation, vous pouvez faire `make clean` avant de faire `make`, cela permet de nettoyer le dossier des anciens fichiers temporaires.
```bash
make clean
```

La compilation génère un fichier exécutable du nom de `executable`. Vous pouvez lancer le programme en faisant :
```bash
./executable
```

### Arguments en ligne de commande

Il est possible de changer les paramètres numériques directement en ligne de commande :

```bash
./executable -patch 3 3 3 -t 20 -it 500 -print 100 -diags 500 -np 5000 -air_damping 0 -gravity 0 0 0 -wall_damping 0 -collision_damping 0 -periodicity 1 -collision 1 -velocity 1E-5 0.1 -x 0 4 -y 0 4 -z 0 4 -r 0.01
```

- `-patch`: nombre de patchs dans chaque direction
- `-t`: temps total
- `-it`: nombre d'itérations
- `-np`: nombre de particules dans le domaine
- `-r`: rayon d'une particule
- `-collision_damping`: friction (perte d'énergie) due aux collisions (par défaut 0)
- `-wall_damping`: friction lors de la collision avec les murs (par défaut 0)
- `-air_damping`: coefficient de frottement de l'air
- `-velocity`: vitesse min et max pour l'initialisation
- `-mass`: masse de chaque particule
- `-x`: bornes du domaine en x
- `-y`: bornes du domaine en y
- `-z`: borne du domaine en z
- `-gravity`: vecteur gravité
- `-print`: période d'affichage dans le terminal en nombre d'itérations
- `-diags`: période de sortie des fichiers en nombre d'itérations
- `-collision`: nombre de collisions par particule à chaque pas de temps


### Visualisation

Le code peut générer plusieurs types de fichiers :
- Fichier VTK : Les fichiers sont créés indépendamment de la bibliothèque VTK à la main pour ne pas imposer de nouvelle dépendance.
Ces fichiers peuvent être visualisés à l'aide des logiciels VisIt ou Paraview. Pour en apprendre plus sur l'utilisation de Paraview, rendez-vous sur cette [page](./visualization/README.md).
- Fichiers binaires : ces fichiers sont un enregistrement binaire des propriétés des particules.
  On peut visualiser ces données avec `matplotlib` en utilisant le script [plot_binary_matplotlib.py](./python/plot_binary_matplotlib.py).
  On peut aussi générer une image 3D grâce au paquet Python Mayavi en utilisant le script [plot_binary_mayavi.py](./python/plot_binary_mayavi.py).
- Fichier HDF5 visualisable via `Python` : pour cela, utilisez les scripts disponible dans le dossier [python](./python).
  Vous avez besoin de python avec la bibliothèque `matplotlib` et `h5py`. Cette sortie est néanmoins désactivée par défaut car elle nécessite l'installation de HDF5.

## Consignes de TP

Dans ce TP, vous aurez à modifier un programme séquentiel afin de le rendre parallèle avec OpenMP puis MPI.
Vous serez ensuite amené à étudier les performances des versions parallèles.
Le TP se compose de 5 sous-sections :
- [Sous-section I](./consignes/1_sequentiel.md) : il s'agit de la découverte du code séquentiel
- [Sous-section II](./consignes/2_machine.md) : cette partie concerne la découverte de la machine parallèle
- [Sous-section III](./consignes/3_omp.md) : cette partie est l'implémentation de la version OpenMP
- [Sous-section IV](./consignes/4_mpi.md) : cette partie est l'implémentation de la version MPI
- [Sous-section V](./consignes/5_performance.md) : cette partie est l'étude de la performance des codes parallèles

En plus de travailler dans un code de calcul, il vous est demandé d'écrire un rapport détaillant votre démarche.
Le TP est divisé en questions successives vous invitant soit à modifier le code soit à compléter votre rapport, parfois les deux.
Pour le rapport, vous êtes libre de choisir le format et le logiciel qui vous convient (LateX, Word, LibreOffice...).

Une section [Aide](./consignes/6_aide.md) vous permettra d'obtenir de l'aide sur certaines questions.
Cette section évolue en fonction des questions reçues.
