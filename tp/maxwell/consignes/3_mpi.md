## III. MPI

Dans cette troisième partie, nous allons paralléliser le programme séquentiel en utilisant la méthode par passage de message et plus spécifiquement le standard MPI.

**Préparation :** Faites maintenant une copie du dossier `sequentiel` et appelez-le `mpi`.
On modifiera les sources de ce dossier pour y introduire la parallélisation MPI.

**Question 3.1 - compilation :** En premier lieu, il faut garder à l'esprit que la compilation se fera maintenant grâce au wrapper MPI:

```bash
mpic++ -O3 main.cpp -o exe
```

On utilise seulement le flag d'optimisation `-O3`.

Il est tout à fait possible de compiler un code séquentiel avec le *wrappper* MPI puisqu'il s'agit simplement d'un *wrapper* faisant appel au compilateur standard (`g++` ou `icpc`ici).
Compilez donc le code sans modification avant de commencer.

**Question 3.2 - Initialisation de MPI :** Nous allons commencer par préparer le programme pour MPI.

a) Commencez par inclure le header MPI dans le fichier [main.cpp](../cpp/main.cpp).
Notez que dans un programme composé de plusieurs sources, il faudrait l'inclure dans chaque fichier où seraient appelées des fonctions MPI.

b) Effectuez l'initialisation de MPI au début du fichier [main.cpp](../cpp/main.cpp).
Rajoutez la déclaration des nouveaux paramètres nécessaires à MPI là où tous les paramètres sont déclarés.
Placez l'initialisation de MPI après la déclaration des paramètre avant la configuration par défaut.

c) Toujours au début de [main.cpp](../cpp/main.cpp), ajoutez les fonctions permettant de récupérer le nombre de rang et le rang du processus en cours.
Aidez-vous du premier exercice sur MPI si besoin `1_initialization`.

d) Ensuite, rajoutez la fonction permettant de finaliser MPI tout de suite à la fin du programme.

e) Pour tester notre programme au fur et à mesure de l'implémentation, commentez la boucle en temps pour le moment.

f) Compilez votre programme sans l'exécuter.
Si des erreurs de compilation apparaissent, corrigez les avant de passer à la suite.

**Question 3.3 - Action réservée au rang 0 :** Il est important de se rappeler que dans un programme MPI, le code que vous écrivez après l'initialisation de MPI est exécuté par tous les rangs.
Néanmoins, la similitude peut être faite avec l'ouverture d'une région parallèle en OpenMP à partir de laquelle le code est exécuté par tous les *threads*.
A partir de là, il est important d'identifier les zones que l'on souhaite être exécutées que par un seul rang.

a) Faites en sorte que seulement le rang 0 puisse créer le dossier `diags` :
```C++
system("mkdir -p diags");
```

b) L'affichage d'information dans le terminal doit également
n'être effectué que par un rang pour la lisibilité.
Faites en sorte que seul le rang 0 puisse faire l'affichage des informations de sortie.
Bien sûr, pour le débogage, rien de vous empêche de demander à un autre rang d'afficher des informations.
N'oubliez pas l'affichage des *timers* à la fin du programme.

c) Dans le section de code `Summary in the terminal` qui fait un bilan à la fin de l'initialisation,
 rajoutez l'affichage des informations relatives à MPI.

d) Compilez le code et exécutez le avec 1 rang MPI pour voir le résultat.

**Question 3.4 - Timers :** Nous allons changer le calcul du temps en remplaçant la fonction `gettimeofday` et rendant le calcul compatible avec le parallélisme MPI.
Chaque rang possèdera son propre *timer*.
Etant donné que ce temps n'est pas forcément le même pour tous en fonction de la charge de travail à traiter, on affichera à la fin dans le bilan des *timers* le temps maximal, minimal et la moyenne sur l'ensemble des rangs.

a) En premier lieu, nous allons remplacer tous les appels à la fonction `gettimeofday` par la fonction MPI `MPI_Wtime()` plus adaptée.

Chaque processus MPI va donc faire un calcul local du temps passé dans chaque partie.
Le bilan des *timers* ne sera affiché que par un seul processus.
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

**Question 3.5 - Topologie :**

Le but va maintenant être de décomposer le domaine de simulation en sous-domaine (parallélisme par décomposition de domaine). 
Chaque sous-domaine peut être vu comme un domaine avec ses propres conditions limites. 
Lorsque le bord d'un sous-domaine coincide avec les bords du domaine global alors les mêmes conditions aux bords s'appliquent (ici réfléchissante).
Lorsque les bords d'un sous-domaine est une frontière avec un autre sous-domaine, il va falloir échanger des données.
La figure ci-dessous illustre l'état d'un domaine utilisant la notion de *stencil* comme c'est le cas ici.

<img src="../../../support/materiel/life_grid_decomposition_0.svg" height="500">

La figure suivant illustre la décomposition de la grille avec une topologie cartésienne.
Il y a dans cet exemple 4 rangs et donc 4 sous-domaines avec chacun leur grille indépendante.
Après la décomposition du domaine, certaines cellules à la frontière avec les autres rangs MPI n'ont plus directement accès au contenu de leurs voisines.

<img src="../../../support/materiel/life_grid_decomposition_1.svg" height="500">

On fabrique donc une nouvelle rangée de cellule tout autour des frontières entre sous-domaines MPI dans le but de stocker les cellules voisines dont le contenu n'est connu que des autres rangs.
Ces rangées se nomment cellules fantômes.