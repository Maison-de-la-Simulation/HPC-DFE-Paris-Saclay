# Installer le cours sur son ordinateur

Les exercices et les TPs fonctionnent sur le super-calculateur Ruche, où
tout est déjà installé. Pour travailler aussi sur votre ordinateur, il
faut installer quelques outils, puis récupérer le dépôt du cours.

## Ce qu'il faut installer

| Outil | Rôle |
|---|---|
| **Git** | Récupérer le dépôt du cours et le mettre à jour |
| **Python 3** (3.10 ou plus récent) | Exécuter les exercices et les TPs |
| **Une implémentation de MPI** (Open MPI) | Fournir la commande `mpirun` et la bibliothèque MPI |
| **NumPy, Matplotlib, SciPy, mpi4py** | Les bibliothèques Python utilisées dans tout le cours |
| **h5py, VTK** (facultatifs) | h5py pour le chapitre HDF5, VTK pour le TP Mandelbulb |
| **HDF5 parallèle** (facultatif) | Écrire un fichier HDF5 en parallèle, pour le chapitre [HDF5](hdf5/index.md) |
| **Visual Studio Code** (conseillé) | Éditer le code, sur votre ordinateur ou à distance sur Ruche |

## 1. Installer Git, Python et MPI

Choisissez l'onglet de votre système :

=== ":material-linux: Linux"

    Sur **Ubuntu** ou **Debian**, dans un terminal :

    ```bash
    sudo apt update
    sudo apt install git python3 python3-venv python3-pip openmpi-bin libopenmpi-dev
    ```

    Le paquet `libopenmpi-dev` est nécessaire pour installer mpi4py.

    ??? note "Autres distributions"

        Sur **Fedora** :

        ```bash
        sudo dnf install git python3 python3-devel openmpi openmpi-devel
        module load mpi/openmpi-x86_64      # à refaire dans chaque terminal
        ```

        Sur **Arch Linux** :

        ```bash
        sudo pacman -S git python openmpi
        ```

=== ":material-apple: macOS"

    On utilise le gestionnaire de paquets [Homebrew](https://brew.sh/).

    1. Installez les outils de développement d'Apple, puis Homebrew, en
       suivant les instructions de [brew.sh](https://brew.sh/) :

        ```bash
        xcode-select --install
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        ```

    2. Installez Git, Python et Open MPI :

        ```bash
        brew install git python open-mpi
        ```

=== ":material-microsoft-windows: Windows"

    Sous Windows, le plus simple est d'utiliser **WSL** (*Windows
    Subsystem for Linux*) : un vrai Linux Ubuntu, intégré à Windows. Vous
    utilisez alors exactement les mêmes commandes que sur Linux et sur
    Ruche.

    1. Ouvrez **PowerShell en tant qu'administrateur** (clic droit sur le
       menu Démarrer, puis *Terminal (administrateur)*) et lancez :

        ```powershell
        wsl --install
        ```

        Redémarrez l'ordinateur quand c'est demandé.

    2. Ouvrez l'application **Ubuntu** depuis le menu Démarrer. Au premier
       lancement, choisissez un nom d'utilisateur et un mot de passe Linux.

    3. Dans ce terminal Ubuntu, suivez l'onglet **Linux** :

        ```bash
        sudo apt update
        sudo apt install git python3 python3-venv python3-pip openmpi-bin libopenmpi-dev
        ```

    Toutes les commandes de la suite de cette page se lancent dans le
    terminal **Ubuntu**.

    :material-book-open-variant: [Installer WSL (Microsoft)](https://learn.microsoft.com/windows/wsl/install)

    ??? note "Sans WSL : MS-MPI"

        Il est possible d'installer MPI directement sous Windows avec
        [Microsoft MPI](https://learn.microsoft.com/message-passing-interface/microsoft-mpi).
        La commande de lancement est alors `mpiexec -n 4` au lieu de
        `mpirun -np 4`. Cette solution est déconseillée pour ce cours :
        les commandes diffèrent de celles de Ruche et des énoncés.

## 2. Récupérer le dépôt du cours

Placez-vous dans le dossier où vous voulez ranger le cours, puis :

```bash
git clone https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay.git
cd HPC-DFE-Paris-Saclay
```

Le dossier `HPC-DFE-Paris-Saclay` contient tout le cours : `exercices/`,
`tp/`, `documentation/`…

!!! tip "Sous Windows avec WSL"

    Clonez le dépôt **dans votre dossier Linux** (par exemple
    `~/HPC-DFE-Paris-Saclay`), et non dans un dossier Windows
    (`/mnt/c/...`) : l'accès aux fichiers y est beaucoup plus rapide.

## 3. Installer les bibliothèques Python

On installe les bibliothèques dans un **environnement virtuel**, propre au
cours, pour ne pas toucher au Python du système :

```bash
# Création de l'environnement (une seule fois)
python3 -m venv .venv

# Activation (à refaire dans chaque nouveau terminal)
source .venv/bin/activate

# Installation des bibliothèques du cours
pip install -r requirements.txt
```

Quand l'environnement est activé, son nom `(.venv)` apparaît au début de
la ligne de commande.

Le fichier `requirements.txt`, à la racine du dépôt, contient toutes les
bibliothèques utilisées dans les exercices et les TPs :

- **nécessaires à tout le cours** : NumPy, Matplotlib, SciPy et mpi4py ;
- **facultatives**, selon l'exercice ou le TP :
    - **h5py**, uniquement pour le chapitre et l'exercice [HDF5](hdf5/index.md) ;
    - **VTK**, uniquement pour le TP Mandelbulb.

Vous pouvez retirer les lignes `h5py` et `vtk` du fichier si vous n'en avez
pas besoin. Chaque TP a aussi son propre fichier `requirements.txt`, si vous
ne voulez installer que ce dont il a besoin :

```bash
pip install -r tp/mandelbulb/requirements.txt
```

!!! note "h5py et l'écriture parallèle"

    La version de h5py installée par pip ne gère pas l'écriture parallèle
    de fichiers HDF5. Elle suffit pour la plupart du cours ; pour la
    version parallèle, voir l'[étape 4](#4-installer-hdf5-en-version-parallele-facultatif).

??? note "Alternative : conda"

    Si vous utilisez déjà [conda](https://conda-forge.org/) (Miniforge,
    Anaconda), vous pouvez tout installer, y compris MPI, avec une seule
    commande, sous Linux comme sous macOS :

    ```bash
    conda create -n hpc -c conda-forge python numpy matplotlib scipy mpi4py openmpi h5py vtk
    conda activate hpc
    ```

## 4. Installer HDF5 en version parallèle (facultatif)

Cette étape n'est nécessaire que pour **l'écriture parallèle** de fichiers
HDF5, vue dans le chapitre [HDF5](hdf5/index.md). Le reste du cours
fonctionne avec le h5py installé à l'étape 3.

L'écriture parallèle demande une version de h5py **compilée avec MPI**,
ce que pip ne fournit pas : il faut d'abord installer la bibliothèque
HDF5 en version parallèle, puis recompiler h5py avec elle. Lancez ces
commandes avec l'environnement `.venv` activé.

=== ":material-linux: Linux"

    Sur **Ubuntu** ou **Debian** :

    ```bash
    # 1. HDF5 parallèle, et les outils h5ls et h5dump
    sudo apt install libhdf5-openmpi-dev hdf5-tools

    # 2. Compilation de h5py avec MPI
    export HDF5_DIR=/usr/lib/$(uname -m)-linux-gnu/hdf5/openmpi
    CC=mpicc HDF5_MPI="ON" pip install --no-binary=h5py --no-cache-dir --force-reinstall h5py
    ```

=== ":material-apple: macOS"

    ```bash
    # 1. HDF5 parallèle (fournit aussi h5ls et h5dump)
    brew install hdf5-mpi

    # 2. Vérification : ce fichier doit exister
    ls $(brew --prefix hdf5-mpi)/lib/libhdf5.dylib

    # 3. Compilation de h5py avec MPI
    export HDF5_DIR=$(brew --prefix hdf5-mpi)
    CC=mpicc HDF5_MPI="ON" pip install --no-binary=h5py --no-cache-dir --force-reinstall h5py
    ```

    `brew --prefix hdf5-mpi` renvoie un chemin **même si le paquet n'est
    pas installé**. Si l'étape 1 a échoué, l'étape 3 s'arrête avec
    `Unable to load dependency HDF5` : d'où la vérification de l'étape 2.

=== ":material-microsoft-windows: Windows"

    Dans le terminal **Ubuntu** de WSL, suivez l'onglet **Linux**.

Les options `--force-reinstall` et `--no-cache-dir` obligent pip à
recompiler h5py, au lieu de garder la version standard installée à
l'étape 3. Vérifiez ensuite que h5py gère MPI :

```bash
python -c "import h5py; print(h5py.get_config().mpi)"   # doit afficher True
```

??? note "Avec conda"

    conda-forge fournit directement une version de h5py compilée avec MPI :

    ```bash
    conda install -c conda-forge "h5py=*=mpi_openmpi*"
    ```

:material-book-open-variant: [h5py et MPI (documentation officielle)](https://docs.h5py.org/en/stable/mpi.html)

## 5. Vérifier l'installation

Vérifiez que MPI est disponible, puis lancez la correction de l'exercice 1
sur 4 processus :

```bash
mpirun --version

cd exercices/mpi/python/initialization/correction
mpirun -np 4 python main.py
```

Vous devez obtenir quatre lignes, dans un ordre quelconque :

```text
Je suis le rang  0 sur 4
Je suis le rang  3 sur 4
Je suis le rang  1 sur 4
Je suis le rang  2 sur 4
```

Si c'est le cas, votre installation est prête.

## 6. Mettre à jour le dépôt

Le cours évolue pendant l'année. Pour récupérer les dernières versions,
depuis le dossier du dépôt :

```bash
git pull
```

!!! warning "Travaillez dans vos propres fichiers"

    Si vous modifiez les fichiers du dépôt, `git pull` peut entrer en
    conflit avec les mises à jour. Le plus simple est de copier un
    exercice avant de le modifier, ou de travailler dans un dossier à part.

## 7. Éditer le code avec VS Code

[Visual Studio Code](https://code.visualstudio.com/) existe pour Linux,
macOS et Windows. Ouvrez le dossier du dépôt avec *Fichier → Ouvrir le
dossier*, ou depuis un terminal :

```bash
code .
```

- Sous **Windows avec WSL**, installez l'extension **WSL** de VS Code,
  puis lancez `code .` depuis le terminal Ubuntu : VS Code s'ouvre sous
  Windows mais travaille dans Linux.
- L'extension **Python** de VS Code ajoute la coloration, la complétion et
  la sélection de l'environnement `.venv`.
- Pour travailler directement sur Ruche, voir
  [Connexion et environnement](super-calculateur/environnement.md).

## Problèmes fréquents

??? question "`mpirun: command not found`"

    MPI n'est pas installé, ou pas dans le `PATH`. Reprenez l'étape 1. Sur
    Fedora, n'oubliez pas `module load mpi/openmpi-x86_64`.

??? question "`There are not enough slots available in the system`"

    Vous demandez plus de processus que votre ordinateur n'a de cœurs.
    Réduisez le nombre de processus, ou autorisez le partage des cœurs avec
    l'option `--oversubscribe` :

    ```bash
    mpirun --oversubscribe -np 8 python main.py
    ```

??? question "L'installation de mpi4py échoue"

    mpi4py a besoin des fichiers de développement de MPI pour s'installer.
    Sous Linux, vérifiez que le paquet `libopenmpi-dev` (Ubuntu) ou
    `openmpi-devel` (Fedora) est installé, puis relancez
    `pip install mpi4py`.

??? question "`ModuleNotFoundError: No module named 'mpi4py'`"

    L'environnement virtuel n'est pas activé dans ce terminal. Relancez
    `source .venv/bin/activate` depuis le dossier du dépôt.

??? question "`Unable to load dependency HDF5` en compilant h5py"

    La bibliothèque HDF5 parallèle n'est pas installée, ou `HDF5_DIR` ne
    pointe pas vers elle. Reprenez l'étape 4 et vérifiez que le fichier
    `libhdf5` existe bien dans le dossier `lib` de `HDF5_DIR`.

??? question "`h5py was built without MPI support, can't use mpio driver`"

    h5py est installé en version standard. Recompilez-le en suivant
    l'étape 4, avec les options `--force-reinstall` et `--no-cache-dir`.

??? question "Sous macOS, un message `shmem: mmap: an error occurred`"

    Cet avertissement d'Open MPI est sans conséquence : le programme
    s'exécute normalement. Pour le faire disparaître, lancez avant
    `mpirun` :

    ```bash
    export TMPDIR=/tmp
    ```
