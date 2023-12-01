# Exercice pour la prise en main du super-calculateur Ruche

Le but de cet exercice est de faire vos premiers pas sur le super-calculateur Ruche.

1) Connectez-vous au super-calculateur Ruche en utilisant votre *login* :

```bash
ssh -XY <login>@ruche.mesocentre.universite-paris-saclay.fr
```

Vous êtes maintenant connecté.e à la frontale (*login nodes*) dans l'espace `$HOME`.
Si c'est votre première connexion avec le mot de passe par défaut, vous devez le changer en utilisant la commande :

```bash
passwd
```

2) Affichez votre quota en faisant :

```bash
ruche-quota
```

3) Rendez-vous dans votre espace de travail `$WORK` qui est plus approprié pour ce cours :

```bash
cd $WORKDIR
```

Vous pouvez vérifier où vous vous trouver en utilisant la commande `pwd`

4) Nous allons maintenant télécharger dans votre espace de travail le repository du cours en faisant :

```bash
wget https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/archive/refs/heads/master.zip
```

Une fois téléchargé, vous pouvez le décompresser en faisant :

```bash
unzip master.zip
```

Vous pouvez maintenant supprimer l'archive.

5) avant de démarrer le cours, nous allons jouer un peu avec les modules. 
Utilisez la commande `module avail` pour afficher tous les modules disponibles et regarder la liste des modules disponibles.


6) Enfin nous allons charger un module Python :

```bash
module load anaconda3/2020.02/gcc-9.2.0
```

7) Regarder tous vos modules chargés en utilisant la commande :

```
module list
```

Vérifiez bien que le module Python Anaconda est chargé.

8) Utilisez la commande `module purge` pour décharger tous les modules.

```
module purge
```

En refaisant un `module list`, vous pouvez vérifier que tous les modules ont bien été déchargés.

9) Le cours étant basé sur Python, j'ai préparer un environnement Anaconda contenant tous les paquets nécessaires. Vous pouvez le charger en faisant :

```bash
source /gpfs/workdir/labotm/Installations/miniconda3/m2dfe_env.sh
```

Dans la suite du cours, vous n'aurez besoin que de cette commande pour charger votre environnement.

10) Nous allons testé que tout fonctionne bien en lançant un premier programme Python.
Écrivez un programme Python écrivant "Hello World" dans un fichier `hello.py` et exécutez-le sur la frontale en faisant :

```bash
python hello.py
```
