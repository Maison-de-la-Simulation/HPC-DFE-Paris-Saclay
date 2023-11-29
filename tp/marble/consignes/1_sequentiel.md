## I. Découverte du code séquentiel

La première partie de ce TP est la découverte du code dans sa version non parallèle.

**Fichier parameters.cpp / .h :**

Ouvrez le fichier [parameters.h](../cpp/patch/parameters.h) et regarder la structure du code.
Ce header contient la définition d'une structure de données pour des paramètres globaux du nom de `Parameters`.
Les paramètres globaux du code permettent de décrire les propriétés de la simulation.
On trouve les sections suivantes :
- `Time properties` : tout ce qui se réfère au temps de calcul
- `Domain properties` : propriété spatiale et physique du domaine comme sa taille par exemple ou le niveau de gravité.
  Dans la verison par *patch*, on y trouve aussi la façon de décomposer le domaine.
- `Particle properties` : propriété générale des particules
- `Diag properties` : paramètres pour les diagnostiques diverses

Cette structure est régulièrement passée en argument des fonctions ayant besoin des données globales.

**Fichier main.cpp :**

Ouvrez le fichier [main.cpp](../cpp/patch/main.cpp) et explorez la structure du code.
La première partie concerne l'initialisation de la simulation :
1. déclaration des variables
2. initialisation des propriétés de la simulation
3. initialisation des *timers*
4. initialisation de la topologie (décomposition du domaine) et des particules présentes dans chaque *patch*
5. création des conditions aux bords (murs)
6. affichage des informations principales dans le terminal pour récapituler la simulation
7. sortie de fichier avant le démarrage de la boucle en temps

La deuxième partie est la boucle en temps elle-même.
Elle se compose des étapes suivantes pour chaque itération en temps :
1. déplacement des particules suivant les équations du mouvement
```C++
particles.push(params);
```
2. application de l'opérateur de collision
```C++
particles.multipleCollisions(params);
```
3. application des conditions limites
```C++
particles.walls(params, walls);
```
4. échange des particules entre *patch*
```C++
particles.exchange(params);
```
5. écriture sur le disque des fichiers de diagnostique (en fonction de la période demandée)
```C++
particles.writeDiags(params);
```
6. calcul des grandeurs globales (réduction)
```C++
particles.getTotalEnergy(params, total_energy);

particles.getMaxVelocity(params, max_velocity);

particles.getTotalParticleNumber(params, particle_number, imbalance);

particles.getTotalCollisionNumber(params, collision_counter);

particles.getTotalExchangeNumber(params, exchange_counter);
```
7. Affichage d'informations dans le terminal (en fonction de la période demandée) incluant l'énergie cinétique totale des particules, le nombre total de particules, la vitesse maximale des particules et le nombre de collisions.


**Fichier particles.cpp / .h:**

Ouvrez les fichiers [particles.h](./cpp/patch/particles.h) et [particles.cpp](./cpp/patch/particles.cpp) pour explorer la structure du code.
Ce fichier contient la description de la classe `Particles`.
Cette classe représente l'ensemble des particules du domaine et contient donc l'ensemble des *patchs*.
Le header montre que la classe `Particles` contient un tableau d'objet `Patch`:
```C++
std::vector <Patch> patches;
```
On retrouve ensuite la définition des fonctions qui s'appliquent sur les patchs et qui sont appelées soient pour l'initialisation :
```C++
// Initialize the topology for each patch
void initTopology(struct Parameters params);

// Initialize the particles for each patch
void initParticles(struct Parameters params);
```
Soit dans la boucle en temps :
```C++
// Equation of movement applied to particles
void push(struct Parameters params);

// Applied the walls to the particles
void walls(struct Parameters params, Walls walls);

// Perform the binary collisions
unsigned int collisions(struct Parameters params);

// Multiple collison iterations
void multipleCollisions(struct Parameters params);

// Exchange particles between patches
void exchange(struct Parameters params);

// Return the total energy in the domain (all patches)
void getTotalEnergy(struct Parameters params, double & total_energy);

// Return the maximal particle velocity in the domain (all patches)
void getMaxVelocity(struct Parameters params, double & max_velocity);

// Return the total number of particles
void getTotalParticleNumber(struct Parameters params, int & total, int & imbalance);

// Return the total number of collisions
void getTotalCollisionNumber(struct Parameters params, int & total);

// Return the total number of exchange
void getTotalExchangeNumber(struct Parameters params, int & total);

// Output specifically the vtk files
void writeVTK(struct Parameters params, unsigned int iteration);

// Write all type of diags
void writeDiags(struct Parameters params, unsigned int iteration);
```

**Fichier patch.cpp / .h :**

Ouvrez les fichiers [patch.h](./cpp/patch/patch.h) et [particles.cpp](./cpp/patch/patch.cpp) pour explorer la structure du code.
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
void initTopology(struct Parameters params, unsigned int id);

// Initialization functions
void initParticles(struct Parameters params);
```

Puis les fonctions permettant de pousser les particules, de gérer les conditions limites et les collisions :
```C++
// Equation of movement applied to particles
void push(struct Parameters params);

// Applied the walls to the particles
void walls(struct Parameters params, Walls walls);

// Perform the binary collisions
unsigned int collisions(struct Parameters params);

// Multiple collison iterations
unsigned int multipleCollisions(struct Parameters params);
```

On y trouve les fonctions destinées à l'échange de particules :
```C++
// Determine particles to exchange
void computeExchangeBuffers(struct Parameters params);

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

// Return the number of collision
int getCollisionNumber();
```

Ces fonctions sont appelées dans la classe `Particles` (voir [Particles.cpp](./cpp/patch/Particles.cpp)).

**Question 1.1 - première exécution :** Maintenant que vous avez une vision globale du code séquentiel, compilez et exécutez-le avec les paramètres par défaut.

**Question 1.2 - visualisation :** L'exécution a généré des fichiers dans le dossier `diags`.
Il y a plusieurs types de fichiers.
Les fichiers avec l'extension `.vtk` doivent être ouvert avec le logiciel `Paraview` ou `VisIt`.
[Cette page dans le dossier visulization](./visualization/README.md) vous aidera à visualiser les résultats.

- a) Utilisez soit les scripts Python fournis dans le dossier [python](./python) pour visualiser les résultats (utilisez le [README](./python/README.md) pour plus d'information) ou Paraview.

**Rapport :** Placez dans le rapport plusieurs images à différentes itérations de simulation.
