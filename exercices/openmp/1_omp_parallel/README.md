# Exercice 1 : Premier programme OpenMP

Ce premier exercice a pour but de créer un premier programme parallèle tout en utilisant certaines
fonctions de la bibliothèque OpenMP.

## Consignes

1. Le programme fait appel à 4 fonctions de la bibliothèque OpenMP permettant de récupérer le nombre de threads maximal et physique ainsi que l'identifiant du thread OpenMP en cours d'exécution. Commencez par compiler et exécuter le programme sans aucune modification. Vous ne tournez que sur un thread (séquentiel).

2. Ajoutez maintenant intelligemment la directive et clause de parallélisation pour afficher à l'écran les informations sur les threads créés. Faites attention, certaines fonctions OpenMP doivent être appelées dans la région parallèle d'autres à l'extérieur.

3. Prenez soin de faire apparaître la clause de partage des variables par défaut (`omp default(shared)`) ainsi que la clause de privatisation des variables ne pouvant être partagées (`omp private`). Pour rappel, une variable partagée ne devrait pas être modifiée dans une région parallèle par plusieurs threads en même temps (concurrence mémoire).
