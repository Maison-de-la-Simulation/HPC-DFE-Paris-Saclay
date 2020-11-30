# Installation de MPI

## Sous Linux

### Prérequis

Assurez-vous d'avoir un compilateur Fortran et C de disponible comme `gfortran` et `gcc`.
Dans le cas contraire installez le grâce au gestionnaire de paquets.

```bash
sudo apt install gfortran gcc

```

### Utilisation des paquets (Debian/Ubuntu)

Il existe une multitude de distribution MPI.
Nous vous proposons ici les plus communes.

**Attention**: il n'ai pas recommandé d'installer plusieurs versions de MPI.
Désinstallez d'abord une ancienne version avant d'en installer une nouvelle.

#### MPICH

```bash
sudo apt-get update
sudo apt install mpich
```

#### OpenMPI

```bash
sudo apt-get install openmpi-bin openmpi-doc libopenmpi-dev
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
