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

6) Regarder tous vos modules chargés en utilisant la commande :

```
module list
````

Vérifiez que tous les modules souhaités sont chargés.

7) Nous allons créer un script qui contiendra tous nos modules pour tous les charger en une seule commande :

Dans un script `~/env_dfe_hpc.sh` :

```bash
module purge
module load anaconda3/2020.02/gcc-9.2.0
module load intel/20.0.2/gcc-4.8.5
module load intel-mpi/2019.8.254/intel-20.0.2
```

Ici `module purge` permet de décharger tous les modules.

Tous les modules peuvent maintenant être chargés en faisant :

```
source ~/env_dfe_hpc.sh
``` 

8) Nous allons maintenant préparer un premier job que nous allons soumettre sur un noeud entier :

Dans le répertoire de votre choix, créez le fichier : `launch.sh`

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --output=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=01:00:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# on charge les modules en un coup
source ~/env_dfe_hpc.sh

# On regarde les propriétés des processeurs
cat /proc/cpuinfo
```

9) Lancez le script en faisant :

```bash
sbatch launch.sh
```

10) Regardez où en est votre job en faisant :

```
squeue -u <login>
```

11) Nous allons maintenant lancer un premier programme MPI sur un noeud. Nous allons utiliser le premier exercice sur MPI. Nous devons dans un premier temps télécharger le repository du cours en faisant :

```bash
git clone https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay.git
```

12) Dans le dossier du premier exercice, nous allons compiler le code avec la commande :

```
mpic++ -O3 main.cpp -o executable
```

13) une fois le code compilé et l'exécutable disponible, nous allons fabriquer un deuxième script de lancement en utilisant 4 unités de calcul: 

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --output=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=4                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=01:00:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# on charge les modules en un coup
source ~/env_dfe_hpc.sh

# On regarde les propriétés des processeurs
srun ./executable
```

14) Lancez ce code en faisant :

```
sbatch ./executable
```

15) Testez les autres exercices