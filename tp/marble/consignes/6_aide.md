## Aide et astuces

Cette partie contient de l'aide sur les différentes questions du projet

**Aide sur la question 3.3.C:**

Concernant les timers et la question 3, vous devez modifier directement les fonctions `start` et `stop` dans le fichier `timers.cpp`. Dans la version sans OpenMP, on utilise gettimeofday pour récupérer le temps. Vous devez remplacer cet appel par la fonction OpenMP `omp_get_wtime`. Par exemple :

```C++
// Start the specified timer
void Timers::start(std::string name) {
    
    #pragma omp master
    {
    
    int id = index(name);
    
    double time = omp_get_wtime();
    
    temporary_times[id] = time;
    
    }
    
}
```

Ici la variable `temporary_times[id]` stocke temporairement le temps pour l'utiliser dans la fonction `timers::stop`.
Vous devez aussi vous assurer que les timers **ne seront appelés que** par le thread `master` en utilisant la directive corresponante. 

**Aide sur la question 3.4a :**

Une fonction qui ne peut être menée en parallèle dans son état est la fonction de diagnostique `particles.writeDiags(params, iteration);`. Je vous laisse cependant réfléchir à la raison pour votre rapport. De fait, cette fonction ne contiendra pas d'appel à openMP et doit être géré par une zone séquentielle `omp master` ou `omp single`. A vous de voir ce qui vous parait le mieux dans cette situation.

**Précisions sur le question 3.4c :**

Ici je vous demande de rendre parallèle des fonctions qui renvoient un scalaire à partir d'une réduction d'un ensemble de données localisées sur chaque processus. Dans l'exemple de code ci-dessous qui correspond à la fonction `Particles::getTotalCollisionNumber` renvoyant le nombre de collision total pour l'itération en cours, on a une boucle sur les patchs. Chaque patch renvoie le nombre de collisions propre à son domaine (dans la variable `local`) via la fonction `patches::getCollisionNumber`. Il faut ensuite sommer la contribution de chaque patch pour obtenir le nombre de collisions total (dans la variable `total`).

```C++
// Return the total number of collisions
void Particles::getTotalCollisionNumber(struct Parameters params, int & total) {
    
    total = 0;
    
    int local;
    
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
       local = patches[i_patch].getCollisionNumber();
       total += local;
    }
}
```

A vous de trouver comment rendre cette fonction compatible avec OpenMP. Vous vous doutez qu'on peut la rendre parallèle en utilisant la clause `reduction`. Cependant comme je le note dans la question j'ai observé des erreurs avec l'opération de réduction avec certains compilateurs. De fait, je vous autorise à rendre cette partie séquentielle si nécessaire.

**Précisions question 4.3.e :**

L'idée est ici de commenter les fonctions qui ne sont pas encore parallélisées pour pouvoir compiler le code dans la question 4.3.f et ensuite. L'idée est de rendre parallèle l'ensemble du code au fur et à mesure et du coup de décommenter les parties parallélisées étape par étape tout en compilant et exécutant pour vérifier que ça marche. En adoptant cette démarche vous évitez les bugs dans les parties que vous n'avez pas encore modifiées.

En vous demandant ça je vous oblige aussi à réfléchir en amont aux fonctions qui seront impactées par MPI. C'est à la fois pour la démarche de parallélisation et pour vous obliger à prendre du recul.

Libre à vous d'ignorer cette question car normalement si vous arrivez à la fin de la partie MPI toutes les fonctions devront marcher avec MPI et être par conséquent décommentées.

**Aide question 4.5.b :** On utilise tout simplement la fonction `MPI_Wtime()` comme dans les exercices. Par exemple :
```C++
// Start the specified timer
void Timers::start(std::string name) {
    
    int id = index(name);
    
    temporary_times[id] = MPI_Wtime();
    
}
```

**Précisions question 4.5 :**

1. Faut-il modifier `timers.h` ?

Il n'est pas nécessaire de modifier `timers.h` mais vous pouvez pour y ajouter les tableaux pour les temps min, max et moyens.
Les appels aux fonctions MPI pour la gestion des timers doivent se faire dans `timers.cpp` par contre.
Aussi, elles doivent figurer dans la fonction `timers::print` car c'est à ce moment là qu'on fait le bilan.
Dans le tableau  accumulated_times, chaque index représente le temps cumulé pour une partie spécifique du code.
Le premier index est pour l'initialisation, le deuxième pour la boucle ne temps et ainsi de suite en fonction de comment vous créer les timers dans `main.cpp`.
Il faut donc appliquer les fonctions MPI pour chaque index.

Je vous mets un squelette pour la fonction `timers::print` qui pourra vous aider :

```C++
void Timers::print(struct Parameters params) {

    double percentage;
    
    std::vector <double> maximum_times(names.size());
    std::vector <double> minimum_times(names.size());
    std::vector <double> average_times(names.size());
    
    // Fonctions MPI et autres à compléter
    // ...
    // ...
    
    if (params.rank == 0) {
        std::cout << " ------------------------------------ "<< std::endl;
        std::cout << " TIMERS"<< std::endl;
        std::cout << " ------------------------------------ "<< std::endl;
        std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
        std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;

        std::cout << " " << std::setw(20) << names[0] ;
        std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[0];
        std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[0] ;
        std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[0] ;
        std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8)  << "       - %";
        std::cout << " | " ;
        std::cout << std::endl;

        for (int i = 1 ; i < names.size() ; i++) {
            
            percentage = average_times[i] / (average_times[1]) * 100;
            
            std::cout << " " << std::setw(20) << names[i] ;
            std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[i];
            std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[i] ;
            std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[i] ;
            std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8)  << percentage << " %";
            std::cout << " | " ;
            std::cout << std::endl;
        }
    }
}
```

**Aide question 4.6.a :** Dans cette question on modifie le constructeur pour n'avoir plus qu'un patch par rang MPI et donc par objet Particles. Il y a plusieurs solutions. Soit on garde un vecteur de patch mais avec qu'un seul patch dedans :
```C++
// Constructor for Particles
Particles::Particles(struct Parameters params)
{
    patches.resize(1);
    
};
```
Soit on ne déclare qu'un patch tout simplement dans ` Particles.h` au lieu d'un vecteur :
```C++
private:

// single patch
Patch patch;
```

**Aide question 4.6.c :** Le but ici est d'éviter d'utiliser un nombre de processus MPI incohérent avec le nombre total de patch dans chaque direction :
``` C++
if (params.number_of_ranks != params.n_patches_x * params.n_patches_y * params.n_patches_z) {
    std::cerr << " CONFIGURATION ERROR: the total number of patches must match the topology: " << params.number_of_ranks << " " << params.n_patches_x << " " << params.n_patches_y  << " " << params.n_patches_z << std::endl;
    exit(0);
}
```

**Aide question 4.6.d :** Dans `Parameters.h`, il faut déclarer les paramètres suivants :
``` C++
int number_of_ranks; // Number of MPI ranks
int rank;            // Number of MPI ranks
int ranks_per_direction[3]; // Nnumber of ranks for each direction : x, y, z
int periodicity[3]; // Nnumber of ranks for each direction : x, y, z
int reorganisation;
MPI_Comm cartesian_communicator;
int rank_coordinates[3];
int * topology_map;
int exchange;
```
Pour les utiliser dans les fonctions de création de la topologie cartésienne.
Par exemple :
``` C++
MPI_Cart_create(MPI_COMM_WORLD,
                     3,
                     params.ranks_per_direction,
                     params.periodicity,
                     params.reorganisation,
                     &params.cartesian_communicator);
```
C'est d'ailleur spour cela qu'on fait un passage par référence ici (question 4.6.b).
