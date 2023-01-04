# Installation de MPI

## Sous Linux

### Prérequis

Assurez-vous d'avoir un compilateur Fortran, C et C++ de disponible comme `gfortran`, `gcc`, `g++`.
Dans le cas contraire installez le grâce au gestionnaire de paquets.

```bash
sudo apt install gfortran gcc g++

```

### Utilisation des paquets (Debian/Ubuntu)

Il existe une multitude de distribution MPI.
Nous vous proposons ici les plus communes.

**Attention**: il n'ai pas recommandé d'installer plusieurs versions de MPI.
Désinstallez d'abord une ancienne version avant d'en installer une nouvelle.

#### OpenMPI

```bash
sudo apt-get install openmpi-bin openmpi-doc libopenmpi-dev
```

#### MPICH

```bash
sudo apt-get update
sudo apt install mpich
```

### Depuis les sources

L'avantage d'une installation depuis les sources est de pouvoir bénéficier d'une
installation performante adaptée à votre système avec les paramètres de votre choix.

#### MPICH

- Commencer par télécharger `MPICH` depuis [cette adresse](http://www.mpich.org/static/downloads/3.3/mpich-3.3.tar.gz) dans le dossier de votre choix.

```bash
# Téléchargement en ligne de commande
wget http://www.mpich.org/static/downloads/3.3/mpich-3.3.tar.gz
```

- Décompressez l'archive là où vous souhaitez installer `MPICH`.

```bash
# Décompression en ligne de commande
tar xvf mpich-3.3.tar.gz
```

- Rentrer dans le dossier tout juste décompressé qui contient les sources

```bash
cd mpich-3.3
```

- Créer un dossier du nom de `build`

```bash
mkdir build
```

- Ne rentrez pas dans `build`. Récupérer le chemin absolu vers le dossier `mpich-3.3` en faisant `pwd`:

```bash
pwd
```

- Depuis la racine du dossier `mpich-3.3` (là où vous êtes normalement), lancer cette commande  qui permet de préparer la compilation :

```bash
./configure --prefix=<chemin absolu vers le dossier mpich-3.3>/build/
```

- Puis dans le même terminal, vous allez maintenant compiler le code :

```bash
make -j 4
```

- Puis finaliser l'installation

```bash
make install
```

- Les fichiers fraichement compilés sont maintenant dans `/build`.
On y trouve les `bin`, les `lib` et les `include` qu'il va falloir passer au système.
Pour cela, on va créer un fichier `configuration` dans `build` que l'on sourcera pour mettre à jour le système

```bash
# Fichier de configuration
export MPICH_HOME=<chemin absolu vers le dossier build>
export PATH=$MPICH_HOME/bin:$PATH
export LD_LIBRARY_PATH=$MPICH_HOME/lib:$LD_LIBRARY_PATH
```

- Pour mettre à jour le terminal courant, faites un source de ce fichier.

```bash
source <chemin absolu vers build>/configuration
```

Cela met à jour uniquement le terminal courant, vous devez répéter cette
opération à chaque nouveau terminal ouvert.

- Pour mettre votre système à jour pour n'importe quel terminal automatiquement,
vous pouvez rajouter la commande source dans le fichier `~/.bashrc`.

## Sous MacOS

### Soluiton 1: installation de GNU et MPI via Homebrew

Tout d'abord, installer les compilateurs GNU en utilisatn la commande:

```bash
brew install gcc
```

Par défaut, les compilateurs GNU ne sont pas les compilateurs par défaut (le compilateur LLVM du système reste celui par défaut).
Pour les utiliser correctement, on va donc prévenir notre système de le faire, on va exporter les variables suivantes :

```bash
export CC=gcc-12
export CXX=g++-12
```

Ensuite, installez OpenMPI en utilisant toujours Homebrew:

```bash
brew install open-mpi
```

A la fin de l'installation, Homebrew vous dit où OpenMPI est installé.
Par exemple pour moi : `/opt/homebrew/Cellar/open-mpi/4.1.4_2`.

On va mettre ce chemin dans un variable dédiée :

```bash
export OPENMPI_HOME=/opt/homebrew/Cellar/open-mpi/4.1.4_2
```

Nous allons maintenant dire mettre OpenMPI dans notre environnement et surtout dire à OpenMPI d'utiliser GNU et non le compilateur du système. Pour cela :

```bash
export PATH=$OPENMPI_HOME/bin:$PATH
export LD_LIBRARY_PATH=$OPENMPI_HOME/lib:$LD_LIBRARY_PATH
export OMPI_CXX=g++-12
```

Le mieux est de mettre tout ça dans un fichier, par exemple `mpi_env.sh` :

```bash
export CC=gcc-12
export CXX=g++-12
export OPENMPI_HOME=/opt/homebrew/Cellar/open-mpi/4.1.4_2
export PATH=$OPENMPI_HOME/bin:$PATH
export LD_LIBRARY_PATH=$OPENMPI_HOME/lib:$LD_LIBRARY_PATH
export OMPI_CXX=g++-12
```

Et de faire un source de ce fichier dans votre `~/.zshrc` pour automatiquement configurer votre terminal à chaque session ou de le faire à la main en faisant :

```bash
source ~/mpi_env.sh
```

### Soluiton 2: installation de LLVM et MPI via Homebrew

Cette solution est très similaire à la précédente mais elle utilise le compilateur LLVM au lieu de GNU.

```bash
brew install llvm
brew install open-mpi
```
Récupérer le chemin vers LLVM après son installation, par exemple pour moi :

```bash
export LLVM_HOME=/opt/homebrew/opt/llvm@14/
```

```bash
# Pour LLVM
export PATH="$LLVM_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$LLVM_HOME/lib":$LD_LIBRARY_PATH
export CC=clang
export CXX=clang++
# Pour OpenMPI
export OPENMPI_HOME=/opt/homebrew/Cellar/open-mpi/4.1.4_2
export PATH=$OPENMPI_HOME/bin:$PATH
export LD_LIBRARY_PATH=$OPENMPI_HOME/lib:$LD_LIBRARY_PATH
export OMPI_CXX=clang++
```

### Solution 3: Passer par Docker pour avoir un environnement Linux

La troisième solution est d'utiliser Docker pour virtualiser une machine Linux.

Pour cela, il faut d'abord installer Docker sur Mac : https://docs.docker.com/desktop/install/mac-install/

Puis charger dans un terminal une image Ubuntu :

```bash
docker pull ubuntu
```

Lancer votre image en faisant :
```
docker run -it ubuntu /bin/bash
```

Utilisez l'option `-v` pour monter dans le docker des dossiers de votre Mac :

```
docker run -v "dossier local":"chemin où le dossier sera monté dans le doker" -it ubuntu /bin/bash
```

Suivez ensuite la procédure d'installation pour Linux-Ubuntu plus haut

### Solution 3: Passer par Docker pour avoir un environnement Linux

Utilisez VirtualBox et une image Ubuntu (https://cdimage.ubuntu.com/focal/daily-live/current/ ) pour démarrer une machine virtuelle.
Si votre Mac est équippé d'un processeur ARM alors il est conseillé d'installer une image Ubuntu pour ARM.