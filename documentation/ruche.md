# Utilisation du super-calculateur Ruche

Ruche est un super-calculateur installé au [Mésocentre du Moulon](http://mesocentre.centralesupelec.fr/) sur le Campus de l'université Paris-Saclay. La partition CPU est composé de 216 noeuds bisocket Intel Cascadelake 6230 piur une puissance total de 0.5 petaflops.

La documentation du calculateur est disponible en ligne : https://mesocentre.pages.centralesupelec.fr/user_doc/

## Architecture d'un super-calculateur

![super-calculateur](../support/materiel/super-calculateur.png)

Un super-calculateur se compose de différentes partitions.

- La partition que l'on appelle le plus souvent *login node* ou *frontal* est celle qui accueille les utilisateurs à la connexion. Les ressources en calcul sur cette partition sont destinées à la gestoon de votre espace (commandes Unix, logiciels légers) et à l'utilisation de script léger et simple (python pour la visualisation par exemple). On ne fait pas tourner de calculs parallèles sur cette partition.
- La partition destinée à exécuter les codes sont séparée. Il peut y en avoir plusieurs en fonction des technologies dsponibles. Dans notre cas nous n'utiliserons que la partition CPU classique.

Contrairement à un ordinateur local, on ne lance pas soit même un calcul sur les ressources du super-calculateur. On soumet nos travaux, que l'on appelle *job* à un ordonenceur ou *job scheduler* qui décide en fonction des ressources disponibles et d'autres critères (durée du *job*, priorité, ressources demandées, etc) quand réaliser l'execution de vos travaux. 

Concrètement, on écrit nos besoins et nos étapes de travail dans un script qui sera ensuite lu et exécuté par l'ordonenceur. 

En terme de stockage, il y a là aussi plusieurs partitions:

- `$HOME` En général, une partition *home* avec peu de mémoire disponible et destionée surtout aux fichiers systèmes, aux scripts et aux bibliothèques légères.
- `$WORK` Une partition *work* est disponible pour les codes, les bibliothèques lourdes et la généraion des données de simulation. C'est comme son nom l'indique la partition de travail.

Pour accéder à l'espace *work*, il suffit de faire :
```bash
cd $WORK
```

## Comment se connecter sur les noeuds de login

## Utilisation des modules

## Créer et lancer un job de calcul

## Gestion des travaux en cours