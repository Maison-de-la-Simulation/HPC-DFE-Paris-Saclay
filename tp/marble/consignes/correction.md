**Question 5.1 - Weak Scaling OpenMP :**

a) Justifiez pourquoi le SCHEDULER `STATIC` est le plus adéquate pour les études OpenMP ?

La charge sur chaque processus est égale et constante.

b) Quels sont les composantes dominantes des parties parallèles et non-parallèles (suivant la définition d'Amdhal) dans cette étude ?

Les collisions dominent la parties parallèles.
Les échanges de particules dominent la partie non-parallèles jusqu'à 30 processus environ.
Les réductions des grandeurs globales deviennent dominantes après.

c) Quel est le comportement du temps passé dans les collisions et le pousseur ? Ce comportement vous paraît-il cohérent avec un *weak scaling* ? Expliquez.

Le temps dans les collisions augmentent avec le nombre de processus tout en restant dans les mêmes ordres de grandeur (entre 9s et 11s).
Le temps dans les équations du mouvement augmente légèrement mais on peut dire qu'il reste quasiment constant.
Normalement, le temps dans ces sections ne devraient pas varié dans une étude de *weak scaling*.
Ils devraient restés constants car la charge est constante sur chaque processus.
Cette augmentation pourrait s'expliquer par les overheads OpenMP bien que les écarts en temps pour les collisions soient très importants pour que cette seule explication suffise.
Il est possible qu'il y ait des effets de cache bien que cela ne soit pas démontré.

d) Quel est le comportement du temps passé dans les échanges de particules et les communications globales ? Ce comportement vous paraît-il cohérent avec un *weak scaling* ? Expliquez.

Le temps dans les échanges augmentent ce qui est cohérent avec un *weak scaling* car le nombre d'éléments à échanger augmente.
Le fait que le temps dans les communications globales augmente très vite s'explique aussi par le fait que ces sections de code sont dans des sections OpenMP *single*.

e) Que peut-on dire du passage à l'échelle du programme OpenMP en observant notamment l'efficacité (Fig 1.2) ?

Avec une efficacité supérieur à 80% sur l'ensemble de l'étude, on peut dire que le code se comporte bien jusqu'à 40 coeurs avec les paramètres numériques proposés.

f) Intuitez quel pourrait être le comportement de l'étude (de l'efficacité notamment) pour un nombre de processus supérieur à 40 et pourquoi ?

L'efficacité devrait continuer à descendre du fait de l'augmentation du temps passé dans les échanges de particule mais surtout dans les communications globales.

**Question 5.2 - Strong Scaling OpenMP :**

a) Comparez le temps passé dans chacune des études et expliquez la différence notamment lorsque le nombre de processus est inférieur à 32 coeurs.

La complexité de l'algorithme de collision est en $N \log{N}$.
Plus le nombre de particules est important plus l'algorithme coûte cher.
En conséquence, dans l'étude 1 qui voit la taille des *patchs* augmenter avec la diminution du nombre de processus, le coût des collisions augmentent plus vite que dans l'étude 2 dans laquelle la taille des *patchs* reste constante.
L'évolution du temps de calcul dans l'étude 2 est d'ailleurs quasiment proportionnel au nombre de processus comme on devrait s'y attendre.

b) Comment peut-on qualifier le comportement de la première étude de Strong Scaling (voir Fig. 2.2). Expliquez ce comportement.

Il s'agit d'une hyper-scalabilité qui s'explique par le fait que le nombre de patch augmente avec le nombre de processus dans l'étude 1.
Les *patchs* sont donc plus petits (moins de charge) et l'algorithme de collision est d'autant plus rapide.

c) Expliquez le comportement linéaire de la deuxième étude de Strong scaling (Fig. 3.2).

Ici le nombre de *patch* et la charge par *patch* restent constants.
Le temps passé dans les parties non-parallèles étant faible, la scalabilité est surtout guidée par le coût du traitement des collisions.
Le coût par *patch* est constant.
Donc le coût total évolue de manière proportionnelle au nombre de processus.

**Question 5.3 - Weak Scaling MPI :**

a) Quelles différences majeures peut-on observer sur les temps entre les deux études (Figs. 4.1 et 5.1) ?

Dans la première étude le coût des communications (échange de particules et globales) est quasiment similaire à celui du traitement des particules (collisions et équations du mouvement).

b) Expliquez les conséquences et la différence sur l'efficacité (Figs. 4.2 et 5.2).

Dans l'étude à 500 particules par *patch*, les coûts des sections non-parallèles (suivant la définition de Amdhal) étant très importantes, l'efficacité du passage à l'échelle chute très rapidement.
On voit que l'on est déjà en dessous de 80% pour plus de 4 processus.
La chute semble néanmoins devenir moins brutale avec l'augmentation du nombre de processus.
Dans la seconde étude à 2000 particules par *patch*, on voit que la capacité du code à passer à l'échelle est bien meilleure.
Le code est toujours proche de 80% d'efficacité sur 2 noeuds soit 40 coeurs.
C'est notamment grâce au temps passé dans les collisions (qui rappelons-le n'est pas linéraire au nombre de particules) qui surpassent celui des échanges et permet ainsi un meilleur passage à l'échelle.

c) Que peut-on dire de la relation entre nombre de particules par patch et capacité du code à passer à l'échelle ?

Plus le nombre de particules sera important, plus les collisions prendront du temps et de fait le temps passé dans les échanges sera négligeable.
En conséquence, l'efficacité du passage à l'échelle ne sera que meilleure même si le temps de calcul augmentera significativement.

**Question 5.4 - Strong Scaling MPI :**

a) Comment peut-on qualifier le comportement de l'étude de Strong Scaling (voir Fig. 6.2). Expliquez ce comportement.

Comme pour le cas OpenMP, on a ici de l'hyper-scalabilité. Les raisons sont similaires.

**Question 5.5 - Comparaison :**

a) Donnez quelles études de strong scaling et weak scaling sont comparables entre le modèle OpenMP et le modèle MPI.

L'étude de weak scaling OpenMP est comparable à la deuxième étude de *weak scaling* MPI.
L'étude de strong scaling MPI est comparable à la première étude de *strong scaling* OpenMP.

b) Comparez le temps et l'efficacité entre les études comparables.

Pour le *weak scaling*, on remarque que les deux méthodes sont relativement efficace avec un avantage sur le modèle OpenMP pour cette étude jusqu'à 40 processeurs.
Les temps de simulations restent du même ordre de grandeur.
Dans le cas OpenMP on observe une augmentation du temps de calcul à cause des collisions malgré le fait que ce comportement n'est pas cohérent avec un weak scaling.
Pour le cas MPI, la chute de scalabilité est bien la conséquence des échanges à la fois entre *patchs* et globaux.

Pour le *strong scaling*, les temps de simulation sont du même ordre de grandeur et la part dans les échanges de particules dominent la composante séquentielle.
La version MPI présentent un comportent un peu mieux en passage à l'échelle comme le montre la courbe d'efficacité.
Les échanges globaux sont un peu plus rapides et le temps passé dans les collisions plus scalable pour ce modèle.
De manière générale, plus les collisions dominent les échanges meilleur est le passage à l'échelle.
