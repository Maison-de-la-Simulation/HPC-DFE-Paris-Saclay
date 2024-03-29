## III. MPI

Dans cette troisième partie, nous allons paralléliser le programme séquentiel en utilisant la méthode par passage de message et plus spécifiquement le standard MPI.

**Préparation :** Faites maintenant une copie du dossier `sequentiel` et appelez-le `mpi`.
On modifiera les sources de ce dossier pour y introduire la parallélisation MPI.

**Question 3.1 - makefile :** En premier lieu, il nous faut modifier le makefile pour pouvoir compiler avec MPI.
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

**Question 3.2 - Initialisation de MPI :** Nous allons commencer par préparer le programme pour MPI.

a) Commencez par inclure le header MPI dans le fichier [main.cpp](../cpp/main.cpp).
Notez que dans un programme composé de plusieurs sources, il faudrait l'inclure dans chaque fichier où seraient appelées des fonctions MPI.

b) Effectuez l'initialisation de MPI au début du fichier [main.cpp](../cpp/main.cpp).
Rajoutez la déclaration des nouveaux paramètres nécessaires à MPI là où tous les paramètres sont déclarés.
Placez l'initialisation de MPI après la déclaration des paramètre avant la configuration par défaut.

c) Toujours au début de [main.cpp](../cpp/main.cpp), ajoutez les fonctions permettant de récupérer le nombre de rang et le rang du processus en cours.
Aidez-vous du premier exercice sur MPI si besoin `1_initialization`.

d) Ensuite, rajoutez la fonction permettant de finaliser MPI tout de suite à la fin du programme.

e) Pour tester notre programme au fur et à mesure de l'implémentation, nous allons commenter la boucle en temps pour le moment.

f) Pour le moment compilez votre programme sans l'exécuter.
Si des erreurs de compilation apparaissent, corrigez les avant de passer à la suite.

**Question 3.3 - Action réservée au rang 0 :** Il est important de se rappeler que dans un programme MPI, le code que vous écrivez après l'initialisation de MPI est exécuté par tous les rangs. Cela diffère d'OpenMP pour lequel le code exécuté en parallèle dépend de l'emplacement des directives.
Néanmoins, la similitude peut être faite avec l'ouverture d'une région parallèle en OpenMP à partir de laquelle le code est exécuté par tous les *threads*.
A partir de là, il est important d'identifier les zones que l'on souhaite être exécutées que par un seul rang.

a) Faites en sorte que seulement le rang 0 puisse créer le dossier `diags`:
```C++
system("mkdir -p diags");
```

b) L'affichage d'information dans le terminal doit également
n'être effectué que par un rang pour la lisibilité.
Faites en sorte que seul le rang 0 puisse faire l'affichage des informations de sortie.
Bien sûr, pour le débogage, rien de vous empêche de demander à un autre rang d'afficher des informations.
N'oubliez pas l'affichage des *timers* à la fin du programme.

c) Dans le section de code `Terminal output summary`, rajoutez l'affichage des informations relatives à MPI.

d) Compilez le code et exécutez le avec 1 rang MPI pour voir le résultat.

**Question 3.4 - Timers :** Nous allons changer le calcul du temps en remplaçant la fonction `gettimeofday` et rendant le calcul compatible avec le parallélisme MPI.
Chaque rang possèdera son propre *timer*.
Etant donné que ce temps n'est pas forcément le même pour tous en fonction de la charge de travail à traiter, on affichera à la fin dans le bilan des *timers* le temps maximal, minimal et la moyenne sur l'ensemble des rangs.

a) En premier lieu, nous allons remplacer tous les appels à la fonction `gettimeofday` par la fonction MPI `MPI_Wtime()` plus adaptée.

Chaque processus MPI va donc faire un calcul local du temps passé dans chaque partie.
Le bilan des *timers* ne seront affichés que par un seul processus.
En revanche, nous allons faire quelques statistiques en affichant le temps minimal, moyen et maximal entre tous les processus pour plusieurs parties du code.

b) Mettre à jour la partie `Summary of the timers` afin d'afficher, par le processus de rang 0 uniquement,  le temps minimal, moyen et maximal calculés entre tous les processus.
Des communications seront nécessaires et vous devrez utiliser les fonctions MPI adéquates.
Il faut en utiliser le moins possible.

Pour l'affichage, vous pouvez utiliser cette portion de code pour remplacer l'entête:

```C++
std::cout << " ------------------------------------ "<< std::endl;
std::cout << " TIMERS"<< std::endl;
std::cout << " ------------------------------------ "<< std::endl;
std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;
```

N'oubliez pas que seul le rang 0 s'occupe de l'affichage.
Par ailleurs, pour le calcul du pourcentage, utilisez le temps moyen.

Pour le moment, seul l'initialisation et la boucle itératives complète sont comptabilisées.
Plus tard, nous ajouterons des *timers* plus précis.

f) Compilez le code et exécutez le en demandant un et plusieurs processeurs pour vérifier que tout fonctionne.
Par exemple:
```bash
mpirun -np 2 ./executable
```

Le but va maintenant être de décomposer le domaine en sous-domaine. 
La figure ci-dessous illustre les différentes étapes et la création des cellules fantômes.
Chaque cellule a besoin du contenu des voisines au temps précédent pour savoir comment évoluer.

<img src="../../../support/materiel/life_grid_decomposition_0.svg" height="500">

La figure suivant illustre la décomposition de la grille avec une topologie cartésienne.
Il y a dans cet exemple 4 rangs et donc 4 sous-domaines avec chacun leur grille indépendante.
Après la décomposition du domaine, certaines cellules à la frontière avec les autres rangs MPI n'ont plus directement accès au contenu de leurs voisines.

<img src="../../../support/materiel/life_grid_decomposition_1.svg" height="500">

On fabrique donc une nouvelle rangée de cellule tout autour des frontières entre sous-domaines MPI dans le but de stocker les cellules voisines dont le contenu n'est connu que des autres rangs.
Ces rangées se nomment cellules fantômes.

<img src="../../../support/materiel/life_grid_decomposition_2.svg" height="1100">

Il faudra à chaque itération utiliser les communications MPI pour remplir ces cellules fantômes à partir des autres rangs.
Les communications doivent se faire par étape, d'abord une direction (x par exemple) puis une autre (y).
De la sorte, les voisins aux coins sont transférés au bon rang en 2 fois comme l'illustre la figure ci-dessus.

**Question 3.5 - Topologie :** Nous allons découpé notre domaine global en sous-domaine.
Chaque sous-domaine sera géré par un rang MPI unique.
Il y aura donc autant de sous-domaines que rangs.
Pour cela, nous allons utiliser la notion de topologie cartésienne vue dans le cours.
Pour cette question, aidez vous de l'exercice MPI 7 sur la construction d'une topologie cartésienne.

Pour rappel, le modèle de décomposition utilisé ici est décrit [dans la présentation du TP](../README.md).

a) Ajoutez la déclaration des paramètres permettant de construire la topologie cartésienne au début de [main.cpp](../cpp/main.cpp).

b) L'appel aux fonctions MPI se fera après la lecture des paramètres en ligne de commande.
Dans la lecture des arguments en ligne de commande, rajoutez un argument `-r` pour récupérer le nombre de rangs dans chaque direction.

```C++
// ...
} else if (key == "-r") {
    ranks_per_direction[0]= atoi(argv[iarg+1]);
    ranks_per_direction[1]= atoi(argv[iarg+2]);
    iarg+=2;
}
```

c) Ajoutez une condition afin de vérifier que le nombre total de rangs est cohérent avec le nombre de rangs par direction spécifié par l'utilisateur. C'est à dire que le nombre de rangs dans la direction x multiplié par le nombre de rangs dans la direction y doit être égal au nombre total de rangs.

d) Ajoutez les fonctions permettant de créer une topologie cartésienne 2D (`MPI_Cart_create`, `MPI_Comm_rank` et `MPI_Cart_coords`).

e) Utilisez `MPI_Cart_shift` pour déterminer les voisins dans les directions `x` et `y`.

**Important :** Je vous rappelle que la convention choisie par les développeurs de MPI fait que la coordonnée continue est la dernière dimension.
Dans ce TP, l'axe continu (indice continu dans le déroulement des boucles) est l'axe des `x`.

f) Déclarez les nouveaux paramètres représentant le nombre de cellules (`nx_loc`, `ny_loc`) ainsi que l'origine de chaque sous-domaine (`origin_x` et `origin_y`).
Initialisez ces paramètres pour chaque processus.

g) Modifiez l'allocation des tableaux pour qu'ils dépendent des paramètres locaux `nx_loc` et `ny_loc` et non des paramètres globaux.

h) Modifiez la phase d'initialisation en conséquence


**Question 3.6 - Parallélisation de la gestion des plantes :** Nous allons dans un premier temps ne nous intéresser qu'aux plantes et laisser les autres espèces commentées.

a) Commencez par mettre à jour les boucles avec les paramètres `nx_loc` et `ny_loc`. Ici, il faut comprendre que chaque processus va traiter sa zone de la même manière que le code séquentiel.

Une fois les plantes calculées et le tableau `plants` actualisé à partir de `new_plants`, il faut faire les communications nécessaires afin de mettre à jour les ghost cells.
Pour cela, nous utiliserons le principe des types dérivés afin de sélectionner une ligne ou une colonne spécifique dans le tableau du rang.

b) Définissez un type dérivé `double_column` et `double_row` représentant respectivement une colonne et une ligne d'un tableau 2D à partir des fonctions MPI adéquates.

c) Ajoutez ensuite après la mise à jour du tableau `plants` mais avant la phase de réduction les processus d'échange nécessaires à la mise à jour des cellules fantômes en utilisant des communications bloquantes ou non-bloquantes selon votre préférence.

d) Mettez à jour la phase de réduction (calcul de `sum_plants_before_rabbits[iteration]`) pour la rendre compatible avec une parallélisation MPI.

e) Ajoutez des nouveaux *timers* afin de calculer le temps passé dans les phases de communication point à point et les phases de communication globale.

f) Compilez et exécutez le code pour vérifier que tout fonctionne

**Question 3.7 - Parallélisation de la gestion des lapins :** nous allons maintenant décommenter la partie qui concerne les lapins afin de la rendre parallèle.

a) Commencez par modifier la fonction `bool outside_domaine` afin de la rendre compatible avec notre décomposition de domaine. Vous devez utiliser les variables `origin_x` et `origin_y` afin de convertir un indice local en indice global.

b) Définissez un type dérivé `int_column` et `int_row` à partir des fonctions MPI adéquates pour les tableaux d'entiers des lapins et des loups.

c) Ajoutez ensuite après la mise à jour du tableau `rabbits`, mais avant la phase de réduction, les processus d'échange nécessaires à la mise à jour des cellules fantômes en utilisant des communications bloquantes ou non-bloquantes selon votre préférence.

d) Mettez à jour la phase de réduction pour la rendre compatible avec une parallélisation MPI.

e) Appliquez les nouveaux *timers* définis plus haut afin de calculer le temps passé dans les phases de communication point à point et les phases de communication globale.

f) Compilez et exécutez le code pour vérifier que tout fonctionne

**Question 3.8 - Parallélisation de la gestion des loups :** nous allons maintenant décommenter la partie qui concerne les loups afin de la rendre parallèle.

a) Effectuez les mêmes actions que pour les lapins

b) Compilez et exécutez le code pour vérifier que tout fonctionne

**Question 3.9 - timers spécifique:** nous allons rajouter des *timers* afin de calculer le temps passé dans:
- les communications globales
- les communications point à point
- les calculs.

a) Créez les variables et rajoutez le calcul des *timers* dans le code autour des zones souhaitées

b) Mettez à jour l'affichage des *timers* à la fin du programme

**Question 3.10 - Sortie des diagnostiques :** Il ne nous reste plus qu'à mettre à jour la partie chargée de la sortie des fichiers de diagnostique.
L'écriture parallèle de fichier n'étant pas au programme de ce cours, nous allons utiliser une méthode alternative consistant à rapatrier chaque portion de la grille globale située sur des rangs distincts vers le rang 0. Voici quelques recommandations :

- Pour cette dernière partie, vous devez prendre soin de créer un tableau sur le rang 0 représentant toutes les portions de grilles de tous les rangs. En plus de cela, des tableaux spécifiques représentant le domaine global avant son écriture sur disque sont nécessaires.
- Utilisez la fonction globale MPI adéquate afin que chaque processus puisse envoyer leur portion de grille sur le rang 0.
- Reconstituez la grille globale à partir des différentes portions reçues précédemment.
- Modifiez le processus d'écriture existant pour le rendre compatible avec la version MPI et les tableaux nouvellement créés.

a) Cette méthode a l'avantage d'être très pédagogique mais présente de nombreux défauts qui font qu'elle ne doit pas être utilisée dans une application scientifique. D'après vous, quels sont les défauts de cette méthode ?
