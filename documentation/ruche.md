# Utilisation du super-calculateur Ruche

Ruche est un super-calculateur installé au [Mésocentre du Moulon](http://mesocentre.centralesupelec.fr/) sur le Campus de l'université Paris-Saclay. La partition CPU est composé de 216 noeuds bisocket Intel Cascadelake 6230 pour une puissance totale de 0.5 petaflops.

La documentation du calculateur est disponible en ligne : https://mesocentre.pages.centralesupelec.fr/user_doc/

## Architecture d'un super-calculateur

![super-calculateur](../support/materiel/super-calculateur.png)

Un super-calculateur se compose de différentes partitions.

- La partition que l'on appelle le plus souvent *login node* ou *frontal* est celle qui accueille les utilisateurs à la connexion. Les ressources en calcul sur cette partition sont destinées à la gestoon de votre espace (commandes Unix, logiciels légers) et à l'utilisation de script léger et simple (python pour la visualisation par exemple). On ne fait pas tourner de calculs parallèles sur cette partition.
- La partition destinée à exécuter les codes sont séparée. Il peut y en avoir plusieurs en fonction des technologies dsponibles. Dans notre cas nous n'utiliserons que la partition CPU classique.

Contrairement à un ordinateur local, on ne lance pas soit même un calcul sur les ressources du super-calculateur. On soumet nos travaux, que l'on appelle *job* à un ordonnanceur ou *job scheduler* qui décide en fonction des ressources disponibles et d'autres critères (durée du *job*, priorité, ressources demandées, etc) quand réaliser l'execution de vos travaux. 

Concrètement, on écrit nos besoins et nos étapes de travail dans un script qui sera ensuite lu et exécuté par l'ordonnanceur. 

En terme de stockage, il y a là aussi plusieurs partitions:

- `$HOME` En général, une partition *home* avec peu de mémoire disponible et destionée surtout aux fichiers systèmes, aux scripts et aux bibliothèques légères.
- `$WORK` Une partition *work* est disponible pour les codes, les bibliothèques lourdes et la généraion des données de simulation. C'est comme son nom l'indique la partition de travail.

Pour accéder à l'espace *work*, il suffit de faire :
```bash
cd $WORK
```

Vous pouvez savoir le taux d'utilisation de vos espaces de stockage en faisant :

```bash
ruche-quota -u <login>
```

Où `<login>` doit être remplacé par votre login.


## Comment se connecter sur les noeuds de login

La connexion à Ruche se fait par ssh en utilisant la commande :

```bash
ssh -XY <login>@ruche.mesocentre.universite-paris-saclay.fr
```

Où `<login>` doit être remplacé par votre login.

Il est également possible de se connecter depuis un PC windows en utilisant le terminal Ubuntu, le logiciel Putty ou la machine virtuelle du Master. Plus d'information sur [cette page](https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/03_connection_and_file_transfer/).

## Utilisation des modules

Un grand nombre de bibliothèques et logiciels sont déjà installés sur le super-calculateur. Il est préférable de les utiliser pour obtenir les meilleures performances. Nous allons donc utiliser les compilateurs et une bibliothèque MPI déjà disponibles.

Par défaut, aucun logiciel et bibliothèque n'est disponible dans votre environnement. Il faut les charger grâce à l'outil module. Les modules permettent d'adapter l'environnement à l'application que vous voulez utiliser.

La commande `module avail` permet d'obtenir la liste des logiciels et bibliothèques disponibles. Cela inclut les différents compilateurs ou Python par exemple.

```
module avail
```

La commande `module list` vous donne la liste des modules chargés dans votre environnement.

```
module list
```

La commande `module load <logiciel>` permet de charger un logiciel ou une bibliothèque. Par exemple :

```
module load python
```

Plus d'information sur le fonctionnement des modules : https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/04_module_command/ 

## Créer et lancer un job de calcul

Un job est avant tout un script qui décrit les ressources demandées et qu'est-ce que les noeuds de calcul vont devoir effectuer.

Par exemple, voici le contenu d'un script `launch.sh`.

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe
#SBATCH --output=output
#SBATCH --error=error            # fichier qui réceptionne la sortie standard
#SBATCH --ntasks=2                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --nodes=1                 # Nombre de noeuds à exploiter
#SBATCH --time=00:30:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# To clean and load modules defined at the compile and link phases
module purge
module load ...

# echo of commands
set -x

# To compute in the submission directory
cd ${SLURM_SUBMIT_DIR}

# execution
srun ./executable
```

En entête, le mot clé `#SBTACH` permet de donner des informations à l'ordonnanceur comme le nombre d'unités de calcul à utiliser.
On prépare ensuite l'environnement en chargeant les modules nécessaires.
On exécute ensuite notre programmme.
`srun` est un wrapper qui remplace la commande `mpirun` tout en s'adaptant aux informations fournies en entête.

Pour donner notre *job* à l'ordonnanceur, on utilise la commande `sbatch`, par exemple :
```bash
sbatch launcher.sh
```

Voir : https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/06_slurm_jobs_management/ 

## Gestion des travaux en cours

Une fois les jobs lancés et en cours de traitement, il est possible de suivre leur évolution en faisant :

```
squeue -u <login>
```

Un job peut être annulé en utilisant la commande :

```
scancel <job id>
```

Toutes les commandes de gestion des jobs sont ici : https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/06_slurm_jobs_management/

## Les environnements possibles pour les exercices

### GNU 11

```bash
module purge
module load anaconda3/2022.10/gcc-11.2.0
module load gcc/11.2.0/gcc-4.8.5
module load openmpi/4.1.1/gcc-11.2.0
```

### Intel 20

```bash
module purge
module load anaconda3/2022.10/gcc-11.2.0
module load intel/20.0.4/gcc-4.8.5
module load intel-mkl/2020.4.304/intel-20.0.4.304
export I_MPI_CXX=icpc
```

## Transfère de données avec Ruche

Il existe différentes méthodes pour transférer des fichiers depuis son ordinateur vers Ruche ou inversement.

### SCP

La commande `scp` utilise le protocole ssh (celui que vous utilisez en ligne de commande pour vous connectez à Ruche) pour transférer des données.

Voici comment utiliser cette commande :

```bash
scp <login>@@ruche.mesocentre.universite-paris-saclay.fr:<chemin vers fichier VTK sur ruche> <chemin où le mettre sur votre machine>
```

La commande `scp` utilise le protocole ssh et vous demandera votre mot de passe.

De nombreux sites en ligne vous expliquerons comment cette commande fonctionne.

### RSYNC

La commande `rsync` est utilisée pour faire de la synchronisation de données et peut se connecter à des serveurs distants via le protocole ssh. `rsync` est à prévilégier pour le transfert de masse (gros fichiers ou nombreux petits fichiers) car en cas de coupure, `rsync` est capable de reprendre là vous en étiez.

### Utilisation de FileZilla

FileZilla est un logiciel de transfert de donnée utilisant les protocoles FTP et SFTP pour accéder à des espaces distants.
Il est possible de se connnecter à Ruche via FileZilla en SFTP afin d'y transférer des données ainsi que pour éditer des fichiers par exemple.

Vous trouverez facilement sur le site officiel comment installer FileZilla.
En fonction de votre OS, vous pouvez aussi passer par un magasin d'applicaiton ou un gestionnaire de paquet.

Une fois FileZilla démarré, vous obtenez l'interface suivante :

<img src="../support/materiel/filezilla_interface.png" height="300">

On va d'abord configurer la connexion. 
Pour cela cliquez sur l'icône pointé par la flèche rouge. 

Vous ouvez alors la page suivante :

<img src="../support/materiel/filezilla_interface_2.png" height="300">

Ici ajoutez un nouveau site :
- choisissez la connexion de type SFTP
- ajoutez le nom de domaine de ruche
- ajoutez votre login
- choisissez si vous souhaitez enregistrer le mot de passer ou le retaper à chaque connexion

Une fois les paramètres rentrées, il suffit de cliquer sur connexion pour accéder au serveur distant.

### Utilisation de VSCode sur Ruche par ssh

Il est possible d'utiliser VSCode sur Ruche pour éditer des fichiers directement sur le serveur distant.

Pour cela, il faut d'abord installer VSCode sur votre ordinateur.

Ensuite, il faut installer l'extension SSH Remote sur VSCode.

Une fois l'extension installée, vous pouvez cliquer en bas à gauche sur l'icône suivante :

<img src="../support/materiel/vscode_remote_ssh.png" height="300">

Choisissez ensuite l'option `Remote SSH: Connect to Host...` et entrez l'adresse de Ruche :

```bash
ssh -XY <login>@ruche.mesocentre.universite-paris-saclay.fr
```

Entrez votre mot de passe et vous êtes connecté à Ruche.

Une fois la connexion établie, vous pouvez ouvrir un terminal distant et travailler comme si vous étiez sur Ruche.

Vous pouvez également ouvrir un dossier distant et travailler sur les fichiers directement sur Ruche en cliquant sur `Ouverture de dossier distant`.

