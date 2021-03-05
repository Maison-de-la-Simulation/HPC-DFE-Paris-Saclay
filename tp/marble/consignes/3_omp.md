### III. OpenMP

Dans cette nouvelle partie, nous allons paralléliser le programme en utilisant la bibliothèque OpenMP
fonctionnant par directives.

**Préparation :** Pour cette partie, faites une copie du dossier `patch` contenant le code pour la version séquentielle par *patch* que vous allez appeler `omp`.
Dans la partie suivante du TP, il vous sera demandé de modifier les sources dans le dossier `omp`.

**Question 3.1 - modification du makefile :** Ouvrez le makefile et rajouter l'option permettant de compiler les directives OpenMP :

```Makefile
CPPFLAGS += -O3 -std=c++11 -fopenmp
LDFLAGS += -fopenmp
```

**Question 3.2 - région parallèle :** La première étape consiste à ouvrir correctement la région parallèle.

a) En premier lieu placez dans [main.cpp](./cpp/patch/main.cpp) la directive d'ouverture et de fermeture d'une région
parallèle en OpenMP (`omp parallel`) pour qu'elle englobe la boucle en temps :
```C++
for (int iteration = 1 ; iteration <= params.final_iteration; iteration++) {
    ...
}
```

**Rapport :** Justifiez le choix de l'emplacement de la directive et pourquoi il n'est pas forcément nécessaire d'inclure la phase d'initilisation des particules et de la topologie (`particles.initTopology(params)` et `particles.initParticles(params)`).

b) Faites en sorte que le passage des paramètres soit partagé par défaut (`shared`) et prenez soin de définir en privé (`private`)
les quelques paramètres qui en ont besoin.

**Rapport :** Justifiez le choix des paramètres passés en `private`.

c) Compilez avec OpenMP (sans exécuter) pour vérifier.

**Question 3.3 - temps :** Pour mesurer le temps, il va être nécessaire de remplacer les fonctions `gettimeofday` par la fonction OpenMP
spécifique `time = omp_get_wtime()` (https://www.openmp.org/spec-html/5.0/openmpsu160.html).
Le temps est géré par les *timers* dans le fichier [timers.cpp](./cpp/patch/timers.cpp).
N'oubliez pas que la fonction `time = omp_get_wtime()` renvoie des secondes.

a) Avant tout, rajouter la ligne permettant d'inclure la bibliothèque OpenMP dans [timers.h](./cpp/patch/timers.h)

b) Modifiez maintenant les fonctions dans la classe `timers` pour utiliser `time = omp_get_wtime()`.

c) Faites-en sorte que seul le thread *master* puisse récupérer le temps afin d'éviter la concurrence mémoire sur les *timers*.

d) Compilez avec OpenMP (sans exécuter) pour vérifier.

**Question 3.4 - parallélisation de la boucle :** On va maintenant paralléliser la boucle en temps.
Ici, on répartira les *patchs* sur les différents threads.

a) Dans la boucle en temps de [main.cpp](./cpp/patch/main.cpp), identifiez les portions de code qui ne peuvent être
exécutées en parallèle et nécessitent l'utilisation d'une directive `omp single` ou `omp master`.

**Rapport :** Justifiez soigneusement vos choix

b) Les différentes étapes de la boucle en temps sont définies dans [particles.cpp](./cpp/patch/particles.cpp).
Rajoutez la directive permettant de paralléliser les boucles sur les patchs dans les fonctions le permettant.
Ajoutez également la clause permettant de choisir le *scheduler* au runtime :
```
#pragma omp for schedule(runtime)
```

**Rapport :** Justifiez soigneusement vos choix

c) Les fonctions permettant de calculer certains paramètres globaux que sont :
- `particles.getTotalEnergy(params, total_energy);` pour obtenir l'énergie totale
- `particles.getMaxVelocity(params, max_velocity);` pour obtenir la vitesse de la particule la plus rapide
- `particles.getTotalParticleNumber(params, particle_number, imbalance);` pour obtenir le nombre total de particules
- `particles.getTotalCollisionNumber(params, collision_counter);` pour obtenir le nombre total de collisions nécessite également un traitement supplémentaire.
Ce sont des fonctions de réduction.

Parallélisez les boucles de ces fonctions tout rajoutant la clause permettant de gérer la réduction.

**Attention :** j'ai remarqué quelques problèmes avec ces réductions soient à la compilation soit à l'exécution.
Il est possible de les remplacer par un `omp master` ou l'utilisation de région critique en cas de problème.

d) Compilez avec OpenMP (sans exécuter) pour vérifier que vous n'avez pas d'erreur.

**Questions 3.5 - exécution :** Nous allons maintenant vérifier que le programme OpenMP fonctionne bien.

a) Exportez dans votre environnement les variables pour le nombre de *threads* OpenMP (par exemple `OMP_NUM_THREADS=4`) et
le type *scheduler* ainsi que le nombre de *chunks*. Choisissez pour commencer `OMP_SCHEDULE="static"`. Exécutez le code.

b) Comparez les résultats avec le code séquentiel.

**Questions 3.6 - visualisation des résultats :** Visualisez les fichiers de sortie pour vous assurer que les résultats sont identiques
avec la version séquentielle.
