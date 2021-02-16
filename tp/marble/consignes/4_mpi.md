### IV. MPI

Dans cette troisième partie, nous allons paralléliser le programme séquentiel en utilisant la méthode par passage de message et plus spécifiquement la bibliothèque MPI.
Pour cela, nous ferons en sorte que chaque *patch* soit traité par un processus MPI.
Un *patch* sera donc associé à un rang MPI systématiquement.

**Préparation :** Faites maintenant une copie du dossier `patch` et appelez-le `mpi`.
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
Compilez le code en faisant `make` pour vous assurer qu'il n'y a pas d'erreur dans le makefile.

**Question 4.2 - Amélioration de la structure `Parameters` :** Avant d'initialiser MPI, nous allons rajouter les variables pour le parallélisme MPI
dans la structure `Parameters` décrite dans [parameters.h](./cpp/patch/parameters.h).
Rajoutez-y une variable pour stocker le nombre total de rangs MPI (par exemple `number_of_ranks`) et une variable pour le rang en cours (`ranks`).
Notez que vous aurez à rajouter de nouvelles variables au dur et à mesure du développement.

**Question 4.3 - Initialisation de MPI :** Nous allons commencer par préparer le programme à MPI.

a) Commencez par inclure le header MPI dans le fichier [main.cpp](./cpp/patch/main.cpp).
Notez qu'il faudra l'inclure dans chaque fichier où sera appelées des fonctions MPI.

b) Effectuez l'initialisation de MPI au début du fichier [main.cpp](./cpp/patch/main.cpp).

c) Toujours au début de [main.cpp](./cpp/patch/main.cpp), ajoutez les fonctions permettant de récupérer le nombre de rang et le rang du processus en cours.
Les variables très locales comme l'erreur MPI par exemple peuvent être déclarées localement.
Aidez-vous du premier exercice sur MPI si besoin `1_initialization`.

d) Ensuite, rajoutez la fonction permettant de finaliser MPI tout de suite à la fin du programme.

e) Pour tester notre programme au fur et à mesure de l'implémentation, nous allons commenter les appels aux fonctions non parallélisées avec MPI dans [main.cpp](./cpp/patch/main.cpp).
Identifiez les fonctions à commenter dans l'initialisation et la boucle en temps.
**Rapport :** Justifiez votre choix dans votre rapport.

f) Compilez et exécutez votre programme avec un seul rang pour tester son fonctionnement.

**Question 4.4 - Action réservée au rang 0 :** Il est important de se rappeler que dans un programme MPI, le code que vous écrivez après l'initialisation de MPI est exécuté par tous les rangs. Cela diffère d'OpenMP pour lequel le code exécuté en parallèle dépend de l'emplacement des directives.
Néanmoins, la similitude peut être faite avec l'ouverture d'une région parallèle en OpenMP à partir de laquelle le code est exécuté par tous les *threads*.
A partir de là, il est important d'identifier les zones que l'on souhaite être exécutées que par un seul rang.

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
std::cout << "  - number of ranks: " << params.number_of_ranks << std::endl;
```

c) Compilez le code pour vérifier que vous n'avez pas fait d'erreur.
Vous pouvez également l'exécuter avec un seul rang.

**Question 4.5 - Timers :** Avant de rentrer dans le coeur du sujet, nous allons préparer le calcul du temps avec MPI.
La définition des *timers* change du fait de l'utilisation de MPI.
Ici le temps enregistré sera le temps propre à chaque rang.
Etant donné que ce temps n'est pas forcément le même pour tous en fonction de la charge de travail à traiter, il est intéressant d'afficher le temps maximal, minimal et la moyenne sur l'ensemble des rangs.

a) Ouvrez le fichier [timers.cpp](./cpp/patch/timers.cpp).

b) En premier lieu, nous allons remplacer tous les appels à la fonction `gettimeofday` par la fonction MPI `MPI_WTIME()` plus adaptée.

Chaque processus MPI va donc faire un calcul local du temps passé dans chaque partie.
Les bilans temporels ne seront affichés que par un seul processus.
En revanche, nous allons faire quelques statistiques en affichant le temps minimal, moyen et maximal entre tous les processus pour chaque partie du code.

c) Au début de la fonction `Timers::print` de [timers.cpp](./cpp/patch/timers.cpp), déclarez les tableaux `minimum_times`, `average_times` et `maximum_times`.

d) Ajoutez les fonctions MPI permettant de calculer la valeur maximale, minimale et la moyenne des valeurs accumulées dans le tableau `accumulated_times`.

e) Améliorez l'affichage des *timers* en y ajoutant ces temps là au lieu du temps accumulé :
```C++
std::cout << " ------------------------------------ "<< std::endl;
std::cout << " TIMERS"<< std::endl;
std::cout << " ------------------------------------ "<< std::endl;
std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;
```

N'oubliez pas que seul le rang 0 s'occupe de l'affichage.

f) Décommentez l'appel aux *timers* pour l'initialisation et l'affichage final. Compilez le code et exécutez le en demandant qu'un processeur.
```bash
mpirun -np 1 ./executable
```

**Question 4.6 - Topologie :** Pour simplifier le développement, l'idée est de faire en sorte que chaque rang MPI ne possède qu'un patch.
De fait, il n'y aura plus plusieurs patchs dans la classe `Particles` mais un seul.
Avec le paradigme MPI, `Particles` représente maintenant les particules du rang courant puisque
que la classe est dupliquée sur chaque rang (mémoire distribuée).
Il est même possible en fonction des choix de chacun de faire disparaitre la classe `Particles`.

a) Dans [Particles.cpp](../cpp/patch/particles.cpp), modifiez le constructeur pour ne laisser qu'un seul patch dans la clasee `Particles`.

Nous allons maintenant récrire la fonction `Particles::initTopology` dans [particles.cpp](../cpp/patch/particles.cpp) et `Patch::initTopology` dans [patch.cpp](../cpp/patch/patch.cpp) pour créer une topologie MPI à partir des fonctions dédiées.

b) Commencez par modifier les arguments de ces fonctions pour que la structure de donnée `Parameters` soit passée par référence:
```C++
void Patch::initTopology(struct Parameters & params);
void Particles::initTopology(struct Parameters & params);
```
On passe la structure par référence car on modifiera certaines données dans ces fonctions.

c) Ajoutez une condition afin de vérifier que le nombre de *patchs* est égal au nombre de rangs MPI spécifiés.

d) Ajoutez dans `Particles::initTopology` les fonctions permettant de créer une topologie cartésienne 3D (`MPI_Cart_create`, `MPI_Comm_rank` et `MPI_Cart_coords`).
Pour le moment on ne s'occupe pas des voisins.
Vous ajouterez les paramètres adéquates dans la structure de donnée `Parameters`.
Le nombre de processus MPI dans chaque direction $x$, $y$ et $z$ est donné par les paramètres `params.n_patches_x`, `params.n_patches_y` et `params.n_patches_z`
car chaque processus MPI ne possède qu'un patch.
Vous pouvez vous aider de l'exercice 6.

e) Ici nous n'utiliserons pas `MPI_Cart_shift` pour déterminer les voisins car nous avons besoin des voisins en diagonal que nous ne donne pas cette fonction.
Pour ce faire, nous allons simplement générer une carte de la topologie sur l'ensemble des processeurs comme dans l'exercice 6 en utilisant `MPI_Cart_coords`.
Ajoutez la carte de la topologie dans la structure `Parameters`.

**Important :** Je vous rappelle que la convention choisie par les développeurs de MPI fait que la coordonnée continue est la dernière dimension.
Dans ce TP, l'axe continu (indice continu dans le déroulement des boucles) est l'axe des $x$.

f) Affichez dans le fichier [main.cpp](./cpp/patch/main.cpp) la topologie à la fin du résumé des paramètres numériques (comme pour l'exercice 6 sur MPI).
Vous pouvez vous inspirer du code suivant :
```C++
std::cout <<  " Topology map: "<< std::endl;

for(int iz = 0; iz < params.ranks_per_direction[0] ; iz++) {
    std::cout << " z = " << iz << std::endl;
    std::cout <<  " ---------------------------> x"<< std::endl;
    for(int iy = 0; iy < params.ranks_per_direction[1] ; iy++) {
        for(int ix = 0; ix < params.ranks_per_direction[2] ; ix++)
        {

            std::cout << " | " << std::setw(3) << params.topology_map[iz*params.ranks_per_direction[1]
            * params.ranks_per_direction[2] + iy*params.ranks_per_direction[2] + ix] ;

        }
        std::cout << "" << std::endl;
    }
    std::cout << " v" << std::endl;
    std::cout << " y" << std::endl;
}
std::cout << std::endl;
```

g) Nous allons maintenant modifier la fonction `Patch::initTopology` ([patch.cpp](./cpp/patch/patch.cpp)) pour prendre en compte les coordonnées MPI dans la configuration de chaque *patch*.
Commencez par mettre à jour la définition des variables suivante :
- `this->id` qui représente l'index du patch
- `id_x`, `id_y`, `id_z` qui représente les coordonnées du patch dans la topologie
Le calcul de la taille du *patch* et des bornes maximales et minimales reste inchangé :

```C++
this->patch_x_length = (params.xmax - params.xmin) / params.n_patches_x;
this->patch_y_length = (params.ymax - params.ymin) / params.n_patches_y;
this->patch_z_length = (params.zmax - params.zmin) / params.n_patches_z;

xmin = id_x * patch_x_length;
xmax = (id_x+1) * patch_x_length;
ymin = id_y * patch_y_length;
ymax = (id_y+1) * patch_y_length;
zmin = id_z * patch_z_length;
zmax = (id_z+1) * patch_z_length;
```

h) Il faut maintenant mettre à jour le calcul des voisins dans le tableau `neighbor_indexes[k]`.
On peut voir que cette partie du code utilise la fonction `Patch::getNeighborIndex`.
Cette fonction renvoie l'index ou le rang du *patch* de coordonnées relatives `id_x + x_shift`, `id_y + y_shift` et `id_z + z_shift`.
On tourne ainsi autour du *patch* courant pour déterminer les rangs voisins.
La fonction `Patch::getNeighborIndex` utilise la fonction `Patch::patchCoordinatesToIndex` qui à partir des coordonnées donne le rang du patch dans la topologie.
Mettez à jour cette fonction pour prendre en compte la topologie `params.topology_map`.
Dans la fonction `Patch::getNeighborIndex`, faites en sorte que `MPI_PROC_NULL` soit la valeur du rang renvoyé par défaut:
```C++
int Patch::getNeighborIndex(struct Parameters params, int x_shift, int y_shift, int z_shift) {

    int index = MPI_PROC_NULL;

    if ((id_x + x_shift >= 0) && (id_x + x_shift < params.n_patches_x ) &&
        (id_y + y_shift >= 0) && (id_y + y_shift < params.n_patches_y ) &&
        (id_z + z_shift >= 0) && (id_z + z_shift < params.n_patches_z )) {

        int index_2;
        patchCoordinatesToIndex(params, index_2, id_x + x_shift, id_y + y_shift, id_z + z_shift);
        index = index_2;
    }
    return index;
}
```
`MPI_PROC_NULL` est compris par MPI comme étant un rang inexistant.
Pour compiler, vous devrez mettre à jour l'ensemble des appels à `Patch::getNeighborIndex`.

i) Expliquez pourquoi la fonction `Patch::patchIndexToCoordinates` qui renvoyait les coordonnées d'un *patch* à partir de son index n'a plus lieu d'être ici ?

La dernière partie de la fonction `Patch::initTopology` pour établir si le *patch* se trouve au bord ne requiert pas de modification.

j) Décommentez l'appel à la fonction `Particles::initTopology` dans [main.cpp](./cpp/patch/main.cpp).
Compilez et exécutez le code avec plusieurs processus MPI cette fois.
N'oubliez pas de spécifier un nombre de *patchs* cohérent avec le nombre total de processus MPI demandé.

**A ce stade, vous avez maintenant correctement initialisé la topologie avec MPI.**

**Question 4.7 - diagnostiques :**
Avant de rendre parallèle la gestion des particules, nous allons nous occuper des diagnostiques.
L'écriture parallèle étant hors programme, nous adoptons ici une méthode peu efficace mais pédagogique qui consiste à ne laisser qu'un processus écrire les données.
L'écriture des particules dans un fichier se fera par le processus 0.
Les autres processus devront envoyer la liste de leurs particules au rang 0.
Le rang 0 devra donc réceptionner l'ensemble dans un tableau destiné à être ensuite écrit dans un fichier.
L'écriture devra respecter les formats utilisés pour la compatibilité avec les scripts.

a) **Mise à jour de la fonction Particles::writeDiags :** Cette fonction appelle deux autres fonctions destinées à écrire sous forme de fichiers la liste des particules et leurs propriétés :
- `Particles::writeVTK` : écriture des fichiers VTK pour Paraview
- `Particles::writeBinary` : écriture des fichiers binaires pour les scripts Python

Vous allez devoir modifier l'ensemble de ces fonctions.
Cette étape ressemble fortement à ce qui a été fait dans l'exercice 6 sur MPI.
Dans `Particles::writeDiags`, créez un tableau pour contenir la liste des particules que contient chaque rang MPI. Utilisez la bonne fonction MPI pour mettre à jour les valeurs sur le rang 0.
Ce tableau est nécessaire pour les futures communications. Il permet au rang 0 de connaître le nombre de particules à recevoir des autres rangs.

b) **Mise à jour de la fonction Particles::writeDiags - suite :** Créez un entier pour contenir la somme de toutes les particules, par exemple :
```C++
int total;
```
Calculez la somme des particules dans tout le domaine sur le processeur 0.

c) **Mise à jour de la fonction Particles::writeDiags - suite :** Allouer des tableaux locaux à la fonction pour stocker les propriétés de l'ensemble des particules qui seront rapatriées sur le processeur 0. Les tableaux seront alloués sur tous les rangs mais la taille sera de 0 dans les rangs supérieurs à 0. Par exemple :
```C++
double * x = new double[total];
```
vous pouvez aussi utiliser la `std::vector` si vous préférez.

- Utilisez les bonnes fonctions MPI pour ramener toutes les propriétés sur le processeur 0.
- Modifiez l'interface des fonctions `Particles::writeVTK` et `Particles::writeBinary` pour passer en argument les propriétés agrégées des particules. Par exemple :
```C++
void Particles::writeVTK(unsigned int iteration, int number_of_particles,
                        double * x, double * y, double * z,
                        double * vx, double * vy, double * vz, double * mass)
```
- Modifiez le coeur de ces fonctions pour écrire les nouveaux tableaux passés en argument.
Dans ces fonctions, il n'est plus nécessaire de boucler sur l'ensemble des *patchs*.
On boucle maintenant sur la liste des particules contenue dans le rang 0.
Par exemple pour les positions dans `Particles::writeVTK`, cela devient :
```C++
// Particle positions
vtk_file << std::endl;
vtk_file << "POINTS "<< number_of_particles << " float" << std::endl;
for(unsigned int ip = 0 ; ip < number_of_particles ; ip++) {
  vtk_file << x[ip] << " " << y[ip] << " " << z[ip] << std::endl ;
}
```
De même cela permet de simplifier la fonction `Particles::writeBinary` où l'écriture des `x` se résume à une ligne de code :
```C++
binary_file.write((char *) &x[0], sizeof(double)*number_of_particles);
```
- Faites en sorte que seul le rang 0 ne s'occupe de l'écriture.
- N'oubliez pas de supprimer les tableaux alloués dynamiquement à la fin de la fonction `Particles::writeDiags` pour éviter les fuites mémoires. Par exemple :
```C++
delete [] x;
```

d) **Mise à jour de la fonction Particles::writeDiags - suite :** décommentez l'appel à la fonction
`Particles::writeDiags` juste avant le démarrage de la boucle en temps.
Cette sortie permet d'obtenir l'état de la simulation avant le démarrage de la boucle en temps.
Compilez et exécutez le code avec plusieurs processus et regardez que le domaine est bien initialisé.

e) **Mise à jour de la fonction Particles::getTotalParticleNumber :** La fonction `Particles::getTotalParticleNumber` est utilisée à plusieurs endroits dans le code pour connaître la somme des particules de tous les rangs dans la simulation ainsi que la différence de charge entre le rang MPI possédant le plus de particules et celui en possédant le moins.
Modifiez cette fonction pour la rendre compatible avec MPI en utilisant la fonction MPI adéquate.
Décommentez l'appel à cette fonction dans [main.cpp](./cpp/patch/main.cpp).

f) **Mise à jour de la fonction Particles::getTotalCollisionNumber :** La fonction `Particles::getTotalCollisionNumber` est utilisée pour connaître la somme des collisions survenues dans tous les rangs de la simulation lors de la dernière itération.
Modifiez cette fonction pour la rendre compatible avec MPI en utilisant la fonction MPI adéquate.
Décommentez l'appel à cette fonction dans [main.cpp](./cpp/patch/main.cpp).

g) **Mise à jour de la fonction particles::getTotalEnergy :** Cette fonction calcule l'énergie cinétique totale contenue dans le système.
Modifiez cette fonction pour la rendre compatible avec MPI en utilisant la fonction MPI adéquate.
Décommentez l'appel à cette fonction dans [main.cpp](./cpp/patch/main.cpp).

h) **Mise à jour de la fonction particles::getMaxVelocity :** Cette fonction calcule l'énergie cinétique totale contenue dans le système.
Modifiez cette fonction pour la rendre compatible avec MPI en utilisant la fonction MPI adéquate.
Décommentez l'appel à cette fonction dans [main.cpp](./cpp/patch/main.cpp).

i) Décommentez l'appel à `Particles::writeDiags` au sein de la boucle en temps. Compilez et exécutez le code avec plusieurs processus pour voir si tout fonctionne.

**Question 4.8 - la boucle de calcul :** On va maintenant réactiver le contenu de la boucle de calcul que l'on a commenté au début de la modification du programme.

a) La plupart des fonctions de la boucle ne nécessite que très peu de modifications car elles sont locales au rang MPI. C'est le cas de `particles::push`, `particles::multipleCollisions`, `particles::walls`.
Il faut simplement supprimer la boucle sur les *patchs* car chaque rang MPI n'a qu'un seul *patch*.

b) Il va maintenant falloir modifier les procédures d'échange pour les particules.
Dans la version séquentielle par *patch*, la procédure est divisée en 3 parties :
- `Patch::computeExchangeBuffers` : On détermine les particules qui sortent du patch courant et on les copie dans des buffers.
  Il y a un buffer par direction d'échange.
  Cette fonction n'a pas besoin d'être modifiée.
  Elle permet maintenant d'identifier les particules qui sortent du rang MPI pour aller vers un autre.
- `Patch::deleteLeavingParticles` : On supprime ensuite les particules du tableau principal qui sortent des limites du rang MPI en cours.
  Comme pour la précédente, cette fonction n'a pas besoin d'être modifiée.
- `Patch::receivedParticlesFromNeighbors` : Cette dernière étape de la procédure d'échange est la communication des particules stockées dans les buffers vers les rangs destinataires.
  C'est cette étape qu'il faudra modifier pour y introduire les fonctions MPI permettant l'échange des particules entre rangs voisins.
  Etant donné que la topologie MPI est proche de la philosophie des *patchs*, on pourra réutiliser l'idée générale pour la gestion des voisins.
  Modifiez `Patch::receivedParticlesFromNeighbors` pour mener à bien les échanges MPI de particules.
  Notez qu'ici il existe plusieurs méthodes possibles plus ou moins efficaces, vous êtes libre de choisir la méthode et les fonctions MPI que vous souhaitez utiliser.

**Rapport :** Expliquez votre démarche en détaillant votre stratégie et vos choix des fonctions MPI.

c) **Mise à jour de Particles::exchange :** Supprimez les boucles sur les *patchs* comme pour les autres fonctions appelées dans la boucle en temps.

**Question 4.9 - vérification :** compilez et exécutez le code en utilisant plusieurs configurations et nombres de processeurs.
Vérifiez que les résultats sont corrects.

**Rapport :** Placer dans le rapport un exemple de simulations en utilisant tous les coeurs de votre ordinateur. Placez des images des résultats en choisissant la méthode de visualisation de votre choix.
