# Simulation de particules par la mécanique du point avec collisions

<img src="../../support/materiel/marble.png" height="400">

## Présentation des modèles

### Mouvement

Ce cas applicatif simule un ensemble de particules (sphère) par les équations classiques de Newton en 3D.
La simulation intègre l'effet des frottements et de la gravité.
Elle intègre également un module de collisions.

L'équation du mouvement qui s'applique à chaque particule est la suivante :
<p align="center"><img src=".extra//3ade19f5b4a6f3d812e3b551a00193ce.svg?invert_in_darkmode" align=middle width=137.90964pt height=38.250465pt/></p>

Où <img src=".extra//8b5bd5fd95868f24ad0a078d34768d7d.svg?invert_in_darkmode" align=middle width=10.747770000000003pt height=23.488740000000007pt/> est le vecteur gravité, <img src=".extra//0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode" align=middle width=14.433210000000003pt height=14.155350000000013pt/> la masse de la particule, <img src=".extra//c745b9b57c145ec5577b82542b2df546.svg?invert_in_darkmode" align=middle width=10.576500000000003pt height=14.155350000000013pt/> un coefficient de freinage
et <img src=".extra//cd74c822d31d457e590f28706c11499d.svg?invert_in_darkmode" align=middle width=10.747770000000004pt height=23.488740000000007pt/> le vecteur vitesse de la particule.

### Collisions

L'opérateur de collision modélise les particules comme des sphères ayant un rayon <img src=".extra//1e438235ef9ec72fc51ac5025516017c.svg?invert_in_darkmode" align=middle width=12.608475000000004pt height=22.46574pt/> donné.
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

### Mouvement

On utilise <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663295000000005pt height=21.683310000000006pt/> pour désigner la i-ème particule dans notre domaine.
On appelle <img src=".extra//4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode" align=middle width=5.936155500000004pt height=20.222069999999988pt/> le temps courant et <img src=".extra//5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode" align=middle width=19.634835000000002pt height=22.46574pt/> le pas en temps.
La discrétisation des équations du mouvement se fait par une méthode explicite classique du type *leap-frog*.
On décompose le vecteur vitesse suivant ses 3 composantes <img src=".extra//7b9f8a1d71e9ae383f2545e50c5370fc.svg?invert_in_darkmode" align=middle width=109.82020499999999pt height=24.65759999999998pt/>.
L'équation du mouvement discrétisée devient :

<p align="center"><img src=".extra//86f029d2718d94d075ea4f3e5474d555.svg?invert_in_darkmode" align=middle width=345.45554999999996pt height=49.31553pt/></p>

De même, on décompose le vecteur position suivant ses 3 composantes <img src=".extra//fcf03b16104b392a5151096287fad486.svg?invert_in_darkmode" align=middle width=88.99440000000001pt height=24.65759999999998pt/>.

<p align="center"><img src=".extra//a120f83e9e76184b2030e4cf29e1fe9c.svg?invert_in_darkmode" align=middle width=169.3362pt height=19.477095pt/></p>

### Collision

Cette partie décrit le fonctionnement de l'opérateur de collision.
On commencer par décrire quelques grandeurs.
On appelle <img src=".extra//4f9e14e83a8a906d39bd1389c1d8d277.svg?invert_in_darkmode" align=middle width=16.438785000000003pt height=33.33363000000001pt/> la vitesse relative entre deux particules :

<p align="center"><img src=".extra//a6b378ac8bec8e1e6929b7843e0ac9f5.svg?invert_in_darkmode" align=middle width=91.325025pt height=19.13241pt/></p>

Cela revient à se placer dans le repère de la première particule.

Et <img src=".extra//debabec6b99de96eb2ea412dac9eb793.svg?invert_in_darkmode" align=middle width=16.674405000000004pt height=33.33363000000001pt/> la direction relative entre les deux positions :

<p align="center"><img src=".extra//1f4f8c38a8b2b2c7eff02e53a68f3fd4.svg?invert_in_darkmode" align=middle width=108.66041999999999pt height=20.776304999999997pt/></p>

Cela revient à placer la première particule au centre de notre repère.

La distance entre 2 particules est donnée par :

<p align="center"><img src=".extra//450d94234a101515c0408cb11a885f03.svg?invert_in_darkmode" align=middle width=69.096555pt height=20.776304999999997pt/></p>

On dit qu'il y a collision entre deux particules lorsque la distance entre les deux centres de masse est inférieure à deux rayons <img src=".extra//52ed010363e449a7d1baf96c0141200f.svg?invert_in_darkmode" align=middle width=56.811645pt height=22.46574pt/>
et lorsque les deux centres continuent de se rapprocher.
Cela revient à dire que dans la direction relative des centres de masse, la vitesse projetée est positive :

<p align="center"><img src=".extra//659268189f92aa2a29c3e11ce44f419c.svg?invert_in_darkmode" align=middle width=77.09443499999999pt height=39.063255pt/></p>

Ces conditions imposent notre pas de temps <img src=".extra//5a63739e01952f6a63389340c037ae29.svg?invert_in_darkmode" align=middle width=19.634835000000002pt height=22.46574pt/> pour prendre en compte toutes les collisions.
Entre chaque pas de temps, la distance relative que deux particules peuvent parcourir ne doit pas dépasser deux rayons <img src=".extra//aadb079978519a78a2e0a1254286d2e7.svg?invert_in_darkmode" align=middle width=20.827785000000002pt height=22.46574pt/>.
Dans le cas contraire certaines particules se traversent sans se collisionner.

<img src="../../support/materiel/collision.png" height="200">

Lorsqu'il y a collision, la vitesse des particules est actualisée suivant la méthode mathématique présentée au-dessus.
On appelle la vitesse post-collision <img src=".extra//eb37fabb7c9f7599d9ef7e121c60b5f5.svg?invert_in_darkmode" align=middle width=16.094100000000005pt height=14.155350000000013pt/>.
Il faut ensuite actualiser les positions.
Etant donné que les particules se chevauchent au moment de la détection de la collision, il faut d'abord revenir en
arrière au moment où les particules rentrent en contact.

Pour cela, on utilise la vitesse avant collision pour déterminer le temps au moment de la collision <img src=".extra//23eda6b0b8aec6ac41ed8b7cb41c0942.svg?invert_in_darkmode" align=middle width=11.810865000000003pt height=20.222069999999988pt/>.
Soit <img src=".extra//88748b5756467c6c663a0d87d2b05caa.svg?invert_in_darkmode" align=middle width=25.509495000000005pt height=22.46574pt/> le laps de temps entre le moment de la collision et le temps de simulation alors <img src=".extra//80ef692b454d0ca3ecad78b37fe23add.svg?invert_in_darkmode" align=middle width=86.08709999999999pt height=22.46574pt/>.
Le but est de déterminer ce fameux <img src=".extra//88748b5756467c6c663a0d87d2b05caa.svg?invert_in_darkmode" align=middle width=25.509495000000005pt height=22.46574pt/> afin de connaître les positions au moment de la collision.
Le moment de la collision est le moment où les particules rentrent en contact soit <img src=".extra//4c49c301c940274cf91c288e6f07968f.svg?invert_in_darkmode" align=middle width=56.811645pt height=22.46574pt/>.

Cela revient à résoudre le système suivant :

<p align="center"><img src=".extra//113efbb57ae11eb51f0b59780cfde951.svg?invert_in_darkmode" align=middle width=269.58689999999996pt height=20.776304999999997pt/></p>

On obtient une équation de second ordre :

<p align="center"><img src=".extra//495e5a4490379876d84f859a26fdce14.svg?invert_in_darkmode" align=middle width=584.4498pt height=20.504055pt/></p>

Les solutions sont nécessairement réelles si les particules se rapprochent. La bonne solution est celle qui est positive.

On calcule donc la position au moment de la collision en faisant simplement : <img src=".extra//a4e6865eacec45964af67e5cbc7f6a4b.svg?invert_in_darkmode" align=middle width=129.528465pt height=33.33363000000001pt/>.
Il s'agit d'une approximation car on ne prend pas en compte la gravité et les frottements.
Une fois la position <img src=".extra//3076985df29f467ae93a0bfbd9d228d3.svg?invert_in_darkmode" align=middle width=16.438785000000003pt height=33.33363000000001pt/> connue, on calcule la position post-collision <img src=".extra//dc2c0845e656d7462673ea305cb1db4e.svg?invert_in_darkmode" align=middle width=18.34305pt height=33.33363000000001pt/> grâce à la vitesse post-collision :

<p align="center"><img src=".extra//90f38fa4891dcd5cd6b9f165ac005301.svg?invert_in_darkmode" align=middle width=131.909745pt height=19.13241pt/></p>

L'opérateur suppose que chaque particule ne collisionne qu'une fois avec une autre particule.
Pour cela, on utilise un algorithme à double boucles imbriquées où la seconde boucle démarre à partir de la particule <img src=".extra//48a0115fc523b1aae58ade9e16001f59.svg?invert_in_darkmode" align=middle width=33.973665000000004pt height=21.683310000000006pt/>.
Soit <img src=".extra//f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.999985000000004pt height=22.46574pt/> le nombre total de particules.
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

Il est possible de simuler des collisions inélastiques (perte d'énergie induite lors de la collision) en multipliant la nouvelle vitesse par un coefficient d'amortissement <img src=".extra//c745b9b57c145ec5577b82542b2df546.svg?invert_in_darkmode" align=middle width=10.576500000000003pt height=14.155350000000013pt/> de telle sorte que la nouvelle énergie de la particule vaut <img src=".extra//d4810e5f7cbd7b818527291de114afa3.svg?invert_in_darkmode" align=middle width=107.69979000000001pt height=26.76201000000001pt/>.

### Conditions aux bords

Les bords sont des murs réfléchissants avec possibilité d'amortissement.
On définit une collision avec un mur au moment où la distance entre le mur et le centre de la particule est inférieur à <img src=".extra//aadb079978519a78a2e0a1254286d2e7.svg?invert_in_darkmode" align=middle width=20.827785000000002pt height=22.46574pt/>.
Un mur est défini par une normal <img src=".extra//21ee6f41257125c58e594746c899b70f.svg?invert_in_darkmode" align=middle width=16.438785000000003pt height=33.33363000000001pt/> et un point <img src=".extra//6820b22b2324e311a04b3d81c2737d02.svg?invert_in_darkmode" align=middle width=16.438785000000003pt height=33.33363000000001pt/>. Par convention, la normale est orientée vers l'intérieur du domaine pour le calcul de la réflexion.
Comme pour les collisions entre particules, on calcule le temps au moment exact de la collision, c'est à dire quand la distance entre le mur et la particule vaut exactement <img src=".extra//aadb079978519a78a2e0a1254286d2e7.svg?invert_in_darkmode" align=middle width=20.827785000000002pt height=22.46574pt/>.
Cela permet de calculer la position de la particule au moment de la collision avant d'actualiser sa vitesse et de calculer la position post-réflexion.

La vitesse post-réflexion <img src=".extra//0f0648ccd31339db8e20252b9e384e34.svg?invert_in_darkmode" align=middle width=16.916130000000003pt height=33.33363000000001pt/> se calcule facilement par réflexion spéculaire :
<p align="center"><img src=".extra//f9bccd016a7101a4e11e0c2339255678.svg?invert_in_darkmode" align=middle width=163.035675pt height=20.776304999999997pt/></p>


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
