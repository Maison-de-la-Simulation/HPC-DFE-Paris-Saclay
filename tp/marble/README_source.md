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
- [cpp/sequential](./cpp/sequential) : ce dossier contient l'ensemble des sources du code séquentiel.
- [cpp/patch](./cpp/patch) : ce dossier contient l'ensemble des sources du code séquentiel avec décomposition en sous-domaine (*patch*).
- [python](./python) : ce dossier contient des scripts dédiés à la visualization et à la comparaison des résultats (fichiers de sortie)
- [.extra](./.extra) : ce dossier sert uniquement pour GitHub

## Le code séquentiel

### Description

Le code est écrit en C++.
C'est un C++ simple n'utilisant pas de capacité avancée du C++.
Néanmoins, il est nécessaire d'être familier avec la notion de classe et de méthode.

Afin de préparer le terrain de la parallélisation, le code séquentiel a été développé en utilisant un modèle de décomposition de domaine.
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
La prise en compte de ce faible nombre de collisions nécessiteraient une complexication du code et des processus d'échange entrainant alors une complexification inutile de ce TP.

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
3. Dans la troisème étape, chaque *patch* va chercher dans les *buffers* de ses voisins les particules qu'ils doivent recevoir.

### Les dépendences

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

### Argument en ligne de commande

### Visualisation

Le code peut générer plusieurs types de fichiers :
- Fichier HDF5 visualisable via `Python` : pour cela, utilisez les scripts disponible dans le dossier [python](./python).
Vous avez besoin de python avec la biblithèque `matplotlib` et `h5py`.
- Fichier VTK : Les fichiers sont créés indépendament de la bilbiothèque VTK à la main pour ne pas imposer de nouvelle dépendance.
Ces fichiers peuvent être visualisés à l'aide des logiciels VisIt ou Paraview. Pour en apprendre plus sur l'utilisation de Paraview, rendez-vous sur cette [page](./paraview.md).

## Consignes de TP

Dans ce TP, vous aurez à modifier un programme séquentiel afin de le rendre parallèle avec OpenMP puis MPI.
Vous serez ensuite amené à étudier les performances des versions parallèles.
Le TP se compose de 5 sous-sections :
- Sous-section I : il s'agit de la découverte du code séquentiel
- Sous-section II :
- Sous-section III :
- Sous-section IV :
- Sous-section V :

En plus de travailler dans un code de calcul, il vous est demandé d'écrire un rapport détaillant votre démarche.
Le TP est divisé en questions successives vous invitant soit à modifier le code soit à compléter votre rapport, parfois les deux.
Pour le rapport, vous êtes libre de choisir le format et le logiciel qui vous convient (LateX, Word, LibreOffice...).

### I. Découverte du code séquentiel

La première partie de ce TP est la découverte du code dans sa version non parallèle.

**Fichier parameters.cpp / .h :**

Ouvrez le fichier [parameters.h](./patch/parameters.h) et regarder la structure du code.
Ce header contient la définition de plusieurs structures de données.
Ces structures contiennent les paramètres globaux du code permettant de décrire les propriétés de la simulation.
On trouve les structures suivantes :
- `TimeProperties` : tout ce qui se réfère au temps comme l'itération courante ou le temps final
- `DomainProperties` : propriété spatiale et physique du domaine comme sa taille par exemple ou le niveau de gravité.
  Dans la verison par *patch*, on y trouve aussi la façon de décomposer le domaine.
- `ParticleProperties` : propriété générale des particules
- `DiagProperties` : paramètres pour les diagnostiques diverses

Ces structures sont régulièrement passées en argument des fonctions ayant besoin des données globales.

**Fichier main.cpp :**

Ouvrez le fichier [main.cpp](./patch/main.cpp) et explorez la structure du code.
La première partie concerne l'initialisation de la simulation :
1. déclaration des variables
2. initialisation des proriétés de la simulation
3. initialisation des *timers*
4. initialisation de la topologie (décomposition du domaine) et des particules présentent dans chaque *patch*
5. création des conditions aux bords (murs)
6. affichage des informations principales dans le terminal pour récapituler la simulation
7. sortie de fichier avant le démarrage de la boucle en temps

La deuxième partie est la boucle en temps elle-même.
Elle se compose des étapes suivantes pour chaque itération en temps :
1. déplacement des particules suivant les équations du mouvement
```C++
particles.push(time_properties, domain_properties);
```
2. application de l'opérateur de collision
```C++
particles.multipleCollisions(collision_counter, time_properties, domain_properties, particle_properties);
```
3. application des conditions limites
```C++
particles.walls(time_properties, domain_properties, walls);
```
4. échange des particules entre *patch*
```C++
particles.exchange(domain_properties);
```
5. écriture sur le disque des fichiers de diagnostique (en fonction de la période demandée)
```C++
particles.writeDiags(time_properties, diag_properties);
```
6. Affichage d'informations dans le terminal (en fonction de la période demandée) incluant l'énergie cinétique totale des particules, le nombre total de particules, la vitesse maximale des particules et le nombre de collisions.


**Fichier particles.cpp / .h:**

Ouvrez les fichiers [particles.h](./patch/particles.h) et [particles.cpp](./patch/particles.cpp) pour explorer la structure du code.
Ce fichier contient la description de la classe `Particles`.
Cette classe représente l'ensemble des particules du domaine et contient donc l'ensemble des *patchs*.
Le header montre que la classe `Particles` contient un tableau d'objet `Patch`:
```C++
std::vector <Patch> patches;
```
On retrouve ensuite la définitions des fonctions qui s'appliquent sur les patchs et qui sont appelelées soient pour l'initilisation :
```C++
// Initialize the topology for each patch
void initTopology(struct DomainProperties domain_properties);

// Initialize the particles for each patch
void initParticles(struct DomainProperties domain_properties,
                    struct TimeProperties time_properties,
                    struct ParticleProperties particle_properties);
```
Soit dans la boucle en temps :
```C++
// Equation of movement applied to particles
void push(struct TimeProperties time, struct DomainProperties domain_properties);

// Applied the walls to the particles
void walls(struct TimeProperties time_properties, struct DomainProperties domain_properties, Walls walls);

// Perform the binary collisions
unsigned int collisions(struct TimeProperties time, struct ParticleProperties particle_properties);

// Multiple collison iterations
void multipleCollisions(unsigned int & collision_counter, struct TimeProperties time, struct DomainProperties domain_properties, struct ParticleProperties particle_properties);

// Exchange particles between patches
void exchange(struct DomainProperties domain_properties);

// Return the total energy in the domain (all patches)
void getTotalEnergy(double & total_energy);

// Return the maximal particle velocity in the domain (all patches)
void getMaxVelocity(double & max_velocity);

// Return the total number of particles
void getTotalParticleNumber(unsigned int & total);

// Output specifically the vtk files
void writeVTK(unsigned int iteration);

// Write all type of diags
void writeDiags(struct TimeProperties time_properties, struct DiagProperties diag_properties);
```

**Fichier patch.cpp / .h :**

Ouvrez les fichiers [patch.h](./patch/patch.h) et [particles.cpp](./patch/patch.cpp) pour explorer la structure du code.
Ces fichiers décrivent la classe `Patch`, c'est à dire une portion du domaine global.
Chaque patch contient des tableaux pour décrire les propriétés des particules :
```C++
// Arrays for the particle positions
std::vector <double> x;
std::vector <double> y;
std::vector <double> z;

// Arrays for the particle velocities
std::vector <double> vx;
std::vector <double> vy;
std::vector <double> vz;

// Array for the mass
std::vector <double> mass;

// Array to tag the particles that leave the domain
std::vector <bool> mask;
```

Chaque patch contient ses propres informations, notamment concernant son emplacement dans la topologie générale :
```C++
// Patch index
unsigned int id;

// Patch index in each direction in the cartesian topology
unsigned int id_x;
unsigned int id_z;
unsigned int id_y;

// Patch length in each direction
double patch_x_length;
double patch_y_length;
double patch_z_length;

// Flag to rapidly get if the patch is at the boundary
bool at_mx_boundary;
bool at_my_boundary;
bool at_mz_boundary;
bool at_px_boundary;
bool at_py_boundary;
bool at_pz_boundary;

// Indexes of the neightbors
std::vector <int> neighbor_indexes;

// Local boundaries of the patch
double xmin;
double xmax;
double ymin;
double ymax;
double zmin;
double zmax;
```

On retrouve aussi les fonctions internes aux patchs.
Il y a celle permettant l'initialisation :
```C++
// Topology
// This function initializes the patch topology :
// - number of patches in each direction
// - id and coordinates of all patches
void initTopology(struct DomainProperties domain_properties, unsigned int id);

// Initialization functions
void initParticles(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties);
```

Puis les fonctions permmettant de pousser les particules, de gérer les conditions limites et les collisions :
```C++
// Equation of movement applied to particles
void push(struct TimeProperties time, struct DomainProperties domain_properties);

// Applied the walls to the particles
void walls(struct TimeProperties time_properties, Walls walls);

// Perform the binary collisions
unsigned int collisions(struct TimeProperties time, struct ParticleProperties particle_properties);

// Multiple collison iterations
unsigned int multipleCollisions(struct TimeProperties time, struct ParticleProperties particle_properties);
```

On y trouve les fonctions destinées à l'échange de particules :
```C++
// Determine particles to exchange
void computeExchangeBuffers(struct DomainProperties domain_properties);

// Delete the particles leaving particles marked by the mask vector
void deleteLeavingParticles();

// Each patch copies the neighbor buffer in their inner particle list
void receivedParticlesFromNeighbors(std::vector<Patch> & patches);
```

Et enfin les fonctions permettant d'effectuer des bilans (réductions) :
```C++
// Return the total energy in the domain
double getTotalEnergy();

// Return the maximal particle velocity
double getMaxVelocity();

// Return the number of particles
unsigned int getParticleNumber();
```

Ces fonctions sont appelées dans la classe `Particles` (voir [Particles.cpp](./cpp/patch/Particles.cpp)).

**Question 1.1 - première exécution :** Maintenant que vous avez une vision globale du code séquentiel. Compilez et exécutez-le avec
les paramètres par défaut.

**Question 1.2 :** L'exécution a généré des fichiers dans le dossier `diags`. Il y a plusieurs types de fichiers.
Les fichiers avec l'extension `.vtk` doivent être ouvert avec le logiciel `Paraview` ou `VisIt`.
[Une petit documentation sur Paraview](./paraview.md) vous aidera à visualiser les résultats.

- a) Utilisez soit les scripts Python fournis dans le dossier [python](./python) pour visualiser les résultats (utilisez le [README](./python/README.md) pour plus d'information) ou Paraview.

- b) Placez dans le rapport plusieurs images à différentes itérations de simulation.

### IV. MPI

Dans cette troisième partie, nous allons paralléliser le programme séquentiel en utilisant la méthode par passage de message et plus spécifiquement la bibliothèque MPI.
Pour cela, nous ferons en sorte que chaque patch soit traité par un processus MPI.
Un patch sera donc associé à un rang MPI systématiquement.

**Préparation :** Faites maintenant une copie du dossier `sequentiel` et appelez-le `mpi`.
On modifiera les sources de ce dossier pour y introduire la parallélisation MPI.

**Question 4.1 - makefile :** En premier lieu, il nous faut modifier le makefile pour pouvoir compiler avec MPI.
Pour cela ouvrez le fichier `makefile` et remplacer `g++` par `mpic++` en tant que compilateur C++ (`CPP`).
Supprimez les *flags* faisant référence à OpenMP : `-fopenmp`.
L'entête du makefile devrait ressembler à ça :
```makefile
# Fortran compiler (MPI wrapper)
CPP = mpic++
```

La variable `CPPFLAGS` se limite à l'optimisation de niveau 3 :
```makefile
# Optimization
CPPFLAGS += -O3
```

Il est tout à fait possible de compiler un code séquentiel avec le *wrappper* MPI puisqu'il s'agit simplement d'un *wrapper* faisant appel au compilateur standard (`g++` ici).
Compilez le code en faisant `make` pour vous assurez qu'il n'y a pas d'erreur dans le makefile.

**Question 4.2 - Création d'une nouvelle structure :** Avant d'initiliser MPI, nous allons créer une
nouvelle structure dans [parameters.h](./cpp/patch/parameters.h).
Appelez cette nouvelle structure `MPIProperties`.
Rajoutez-y une variable pour stocker le nombre total de rangs MPI et une variable pour le rang en cours.
Aidez-vous des autres structures pour créer cette dernière.

**Question 4.3 - Initialisation de MPI :** Nous allons commencer par préparer le programme à MPI.
Pour cela, commencez par inclure le header MPI dans le fichier [main.cpp](./cpp/patch/main.cpp).
Notez qu'il faudra l'inclure dans chaque fichier où sera appelées des fonctions MPI.

Effectuez l'initialisation de MPI au début du fichier [main.cpp](./cpp/patch/main.cpp).
Ajoutez les fonctions permettant de récupérer le nombre de rang et le rang du processus en cours.
Les variables très locales comme l'erreur MPI par exemple peuvent être déclarées localement.
Aidez-vous du premier exercice sur MPI si besoin `1_initialization`.
Ensuite, rajouter la fonction permettant definaliser MPI tout de suite à la fin du programme.

**Question 4.4 - Action réservée au rang 0 :** Il est important de se rappeler que dans un programme MPI, le code que vous écrivez après l'initialisation de MPI est exécuté par tous les rangs. Cela diffère d'OpenMP pour lequel le code exécuté en parallèle dépend de l'emplacement des directives.
Néanmoins, la similitude peut être faite avec l'ouverture d'une région parallèle en OpenMP à partir de laquelle le code est exécuté par tous les threads.
A partir de là, il est important d'identifier les zones que l'on souhaite être exécuté que par un seul rang.
a) Dans le fichier [main.cpp](./cpp/patch/main.cpp) c'est le cas des parties suivantes :
- la création du dossier `diags` :
```C++
system("mkdir -p diags");
```
- l'affichage des informations dans le terminal
Pour éviter que tous les rangs n'effectuent ces actions, ajoutez une condition pour qu'uniquement le rang 0 les fasse.

b) En plus de cela, rajoutez l'affichage du nombre de rangs dans l'onglet `Topology` :
```C++
std::cout << " Topology:" << std::endl;
std::cout << "  - number of ranks: " << mpi_properties.number_of_ranks << std::endl;
```

c) Comilez le code pour vérifier que vous n'avez pas fait d'erreur.
Vous pouvez également l'exécutre avec un seul rang.

**Question 4.5 - Timers :** Avant de rentrer dans le coeur du sujet, nous allons préparer le calcul du temps avec MPI.
La définition des timers change de fait de l'utilisation de MPI.
Ici le temps enregistré sera le temps propre à chaque rang.
Etant donné que ce temps n'est pas forcément le même pour tous en fonction du la charge de travail à traiter, il est intéressant d'afficher le temps maximal, minimal et la moyenne sur l'ensemble des rangs.

a) Ouvrez le fichier [timers.cpp](./cpp/patch/timers.cpp).

b) En premier lieu, nous allons remplacer tous les appels à la fonction `gettimeofday` par la fonction MPI `MPI_WTIME()` plus adaptée.

Chaque processus MPI va donc faire un calcul local du temps passé dans chaque partie.
Les bilans temporels ne seront affichés que par un seul processus.
En revanche, nous allons faire quelques statistiques en affichant le temps minimal, moyen et maximal entre tous les processus pour chaque partie du code.

c) Au début de la fonction `Timers::print` de [timers.cpp](./cpp/patch/timers.cpp), déclarez les tableaux `minimum_times`, `average_times` et `maximum_times`.

d) Ajoutez les fonctions MPI permettant de calculer la valeur maximale, minimale et la moyenne des valeurs accumulées dans le tableau `accumulated_times`.

e) Améliorez l'affichage des timers en y ajoutant ces temps là au lieu du temps accumulé :
```C++
std::cout << " ------------------------------------ "<< std::endl;
std::cout << " TIMERS"<< std::endl;
std::cout << " ------------------------------------ "<< std::endl;
std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;
```

N'oubliez pas que seul le rang 0 s'occupe de l'affichage.

f) Compilez le code et exécutez le en demandant qu'un processeur.
```bash
mpirun -np 1 ./executable
```

**Question 4.6 - Topologie :** Pour simplifier le développement, l'idée est de faire en sorte que chaque rang MPI ne possède qu'un patch.
De fait, il n'y aura plus plusieurs patchs dans la classe `Particles` mais un seul.
Avec le paradigme MPI, `Particles` représente maintenant les particules du rang courant puisque
que la classe est dupliquée sur chaque rang (mémoire distribuée).
Il est même possible en fonction des choix de chacun de faire disparaitre la classe `Particles`.

a) Dans [Particles.cpp](./cpp/patch/particles.cpp), modifiez le constructeur pour ne laisser qu'un seul patch.

Nous allons maintenant récrire la fonction `Particles::initTopology` dans [Particles.cpp](./cpp/patch/particles.cpp) et `Patch::initTopology` dans [Patch.cpp](./cpp/patch/patch.cpp) pour créer une topologie MPI à partir des fonctions dédiées.
b) Commencez par ajouter dans la liste des arguments de ces fonctions la structure de donnée `MPIProperties` :
```C++
void Patch::initTopology(struct DomainProperties domain_properties, struct MPIProperties mpi_properties)
```
N'oubliez pas de modifier également la définitions de `Particles::initTopology` pour transmettre la structure `MPIProperties` jusqu'à la fonction `Patch::initTopology`.

c) Ajoutez dans `Particles::initTopology` les fonctions permettant de créer une topolgie cartésienne 3D (`MPI_Cart_create`, `MPI_Comm_rank` et `MPI_Cart_coords`).
Pour le moment on ne s'occupe pas des voisins.
Vous ajouterez les paramètres adéquates dans la structure de donnée `MPIProperties`.
Aidez-vous de l'exercice 6.

d) Ici nous n'utiliserons pas `MPI_Cart_shift` pour déterminer les voisins car nous avons besoin des voisins en diagonal que nous ne donne pas cette fonction.
Pour ce faire, nous allons simplement générer une carte de la topologie sur l'ensemble des processeurs comme dans l'exercice 6 en utilisant `MPI_Cart_coords`.
Ajoutez la carte de la topologie dans la structure `MPIProperties`.
