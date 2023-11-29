# Installation de HDF5

## Sous Linux

L'installation de HDF5 peut se faire par le gestionnaire de paquet ou par les sources.
Ici je préfère le faire par les sources pour faire une installation personnalisée.

- Commencer par télécharger `HDF5 10` depuis [cette adresse](https://www.hdfgroup.org/downloads/hdf5/source-code/) dans le dossier de votre choix.

- Décompressez l'archive là où vous souhaitez installer `MPICH`.

- Rentrer dans le dossier tout juste décompressé qui contient les sources

```bash
cd hdf5-1.10.*
```

- Créer un dossier du nom de `build`

```bash
mkdir build
```

- Ne rentrez pas dans `build`. Récupérer le chemin absolu vers le dossier `hdf5` en faisant `pwd`:

```bash
pwd
```

- Depuis la racine du dossier `hdf5-1.10.*` (là où vous êtes normalement), lancer cette commande  qui permet de préparer la compilation :

```bash
CC=mpicc FC=mpifort ./configure --enable-parallel --enable-fortran --prefix=<chemin absolu vers hdf5-1.10.*>/build/
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
export HDF5_HOME=<chemin absolu vers le dossier build>
export HDF5_DIR=$HDF5_HOME
export PATH=$HDF5_HOME/bin:$PATH
export LD_LIBRARY_PATH=$HDF5_HOME/lib:$LD_LIBRARY_PATH
```

- Pour mettre à jour le terminal courant, faites un source de ce fichier.

```bash
source <chemin absolu vers build>/configuration
```

Cela met à jour uniquement le terminal courant, vous devez répéter cette
opération à chaque nouveau terminal ouvert.

- Pour mettre votre système à jour pour n'importe quel terminal automatiquement,
vous pouvez rajouter la commande source dans le fichier `~/.bashrc`.
