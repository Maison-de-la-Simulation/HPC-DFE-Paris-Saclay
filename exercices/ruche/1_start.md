# Exercice pour la prise en main du super-calculateur Ruche

1) Connectez-vous au super-calculateur Ruche en utilisant votre *login* :

```bash
ssh -XY <login>@ruche.mesocentre.universite-paris-saclay.fr
```

Vous êtes maintenant connecté.e à la frontale (*login nodes*) dans l'espace `$HOME`.
Nous allons maintenant préparer notre environnement. 

2) Utilisez la commande `module avail` pour afficher tous les modules disponibles.

3) Il nous faut maintenant choisir un compilateur. Nous allons utiliser le compilateur Intel. Nous allons charger pour cela le module `intel/20.0.2/gcc-4.8.5` :

```bash
module load intel/20.0.2/gcc-4.8.5
```

4) Puis charger une version de MPI, pour cela nous allons utiliser le MPI d'Intel :

```bash
module load intel-mpi/2019.8.254/intel-20.0.2
```

5) Enfin nous allons charger un module Python :

```bash
module load anaconda3/2020.02/gcc-9.2.0
```
