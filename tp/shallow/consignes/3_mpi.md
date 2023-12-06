## III. MPI

Dans cette troisième partie, nous allons paralléliser le programme séquentiel en utilisant la méthode par passage de message et plus spécifiquement le standard MPI.

**Préparation :** Faites maintenant une copie du dossier `sequentiel` et appelez-le `mpi`.
On modifiera les sources de ce dossier pour y introduire la parallélisation MPI.

Vous allez maintenant commenter les lignes de code qui ne sont pas encore parallélisées pour introduire la parallélisation MPI au fur et à mesure :

* l'affichage dans le terminale
* les *timers*
* la boucle en temps

**Question 3.1 - Initialisation de MPI :** Nous allons commencer par préparer le programme pour MPI.

a) Commencez par inclure la bibliothèques MPI `mpi4py` au moment de l'importation des autres bibliothèques.
Comme pour les exercices expliciter l'initialisation de MPI.

b) Initialisez MPI au début du script entre la section `Input parameters` et `Initialization`.

c) Ajoutez une variable et la fonction permettant de récupérer le nombre de rangs.

d) Ajoutez une variable et la fonction permettant de récupérer le rang de chaque processus.

e) Ensuite, rajoutez la fonction permettant de finaliser MPI tout de suite à la fin du programme.

f) Il est important de se rappeler que dans un programme MPI, le code que vous écrivez après l'initialisation de MPI est exécuté par tous les rangs.
A partir de là, il est important d'identifier les zones que l'on souhaite être exécutées que par un seul rang.
Décommettez la partie `Terminal summary` et faites en sorte qu'elle ne soit affichée que par le rang 0.
Ajoutez également l'affichage du nombre de rangs MPI.

f) Exécuter le programme avec un seul rang MPI pour vérifier que tout fonctionne jusque là (cela évite de découvrir des erreurs plus tard).

**Question 3.2 - Création de la topologie cartésienne :** Nous allons maintenant à la suite de l'initialisation de MPI construire notre topologie cartésienne 1D.

a) Ajouter les paramètres nécessaires à la construction de la topologie cartésienne au début du programme.

b) Ajouter les fonctions permettant de construire le nouveau communicateur cartésien `cart_comm` à partir du communicateur global `comm`.

c) Ajouter la fonction permettant de récupérer le rang `cart_rank` dans le communicateur cartésien `cart_comm`.

d) Ajouter la fonction permettant de récupérer les coordonnées `cart_coords` dans le communicateur cartésien `cart_comm`.

e) Ajouter la fonction permettant de récupérer les voisins de gauche `left_rank` et de droite `right_rank` dans le communicateur cartésien `cart_comm`.

f) Afin de vérifier que tout fonctionne, ajouter l'affichage des propriétés de chaque processus MPI dans la section `Terminal output summary`.

**Question 3.3 - Décomposition de domaine :** Maintenant que la topologie cartésienne est en place, on va pouvoir découper le domaine en sous-domaines.
Le parallélisme par décomposition de domaine consiste à découper le domaine global en sous-domaines indépendants.
Chaque sous-domaine est géré par un processus MPI unique.
Il y a donc autant de sous-domaines que de processus MPI.
Dans le cas de ce TP, nous allons découper le domaine en sous-domaines de même taille.
Vous pouvez relire l'introduction du TP pour vous rafraîchir la mémoire.

a) Dans la section `Initialization`, déclarez et initialisez une variable `decomposition_size` qui représente en nombre de points le découpage de la grille en sous-domaines.

b) On va maintenant créer les variables `left_ghost_cell` et `right_ghost_cell` qui représentent le nombre de cellules fantômes à gauche et à droite de chaque sous-domaine.
Pour rappel il est inutile de mettre des cellules fantômes aux extrémités du domaine si les conditions aux limites sont réfléchissantes.
Initialisez ces variables en conséquence.

c) Déclarez et initialisez la variable `local_size` qui représente la taille locale de chaque sous-domaine (en nombre de points incluant les points fantômes).

d) Déclarez et initialisez la variable `local_start` qui représente l'origine locale de chaque sous-domaine (en nombre de points) par rapport à l'index global.

e) Mettre à jour la déclaration et l'initialisation du tableau `height` en utilisant non plus `size` mais `local_size`.

f) Faites de même avec les tableaux  `height_m`, `q` et `q_m`.

g) Mettre à jour l'initialisation des conditions limites

h) A la fin de l'initialisation, on calcule la hauteur maximale d'eau. Ce calcul doit maintenant être fait sur chaque sous-domaine puis réduit sur tous les processus MPI. Retravaillez cette portion de code pour qu'elle soit compatible avec le parallélisme MPI.

i) De même faites les mises à jour pour le calcul de la somme des hauteurs d'eau en appelant la fonction MPI adéquate.

j) Mettre à,jour le calcul de la hauteur moyenne `average_height` et de quantité d'eau `water_quantity`

k) Faites en sorte que seul le rang 0 puisse créer le dossier `diags`

l) Mettre à jour l'affichage des informations de sortie dans la section `Terminal output summary` pour donner pour chaque rang les informations précédemment calculées. 

m) Faites tourner le code pour vérifier que tout fonctionne. Vérifiez la cohérence de ce qui est affiché.

**Question 3.4 - Boucle de calcul :** On va maintenant au fur et à mesure décommenter la boucle en temps pour la rendre compatible avec le parallélisme MPI.

a) Commencez par décommenter le calcul des équations en remplaçant `size` par `local_size`

b) Mettre à jour les conditions limites réfléchissantes pour qu'elles ne soient effectuées que par les rangs MPI aux extrémités du domaine.

c) A ce stade la boucle en temps peut tourner mais chaque sous-domaine est indépendant. Il faut maintenant ajouter les communications nécessaires afin de mettre à jour les cellules fantômes. Utilisez les communications point à point pour cela (bloquantes ou non en fonction de votre préférence).

d) Mettre à jour la section de code `# Terminal information` qui affiche dans le terminal à interval régulier l'état de la simulation.

e) Faites tourner la simulation pour vérifier que tout fonctionne

**Question 3.5 - Mesure du temps :** 

**Question 3.6 - sortie des diagnostiques :** Il ne nous reste plus qu'à mettre à jour la partie chargée de la sortie des fichiers de diagnostique (section de code `# Output`).
