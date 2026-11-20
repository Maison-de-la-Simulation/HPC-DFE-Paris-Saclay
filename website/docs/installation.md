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
| **NumPy, Matplotlib, mpi4py** | Les bibliothèques Python utilisées dans le cours |
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

# Installation des bibliothèques
pip install numpy matplotlib mpi4py
```

Quand l'environnement est activé, son nom `(.venv)` apparaît au début de
la ligne de commande.

Certains TPs utilisent d'autres bibliothèques. Chacun a son fichier
`requirements.txt` :

```bash
pip install -r tp/mandelbulb/requirements.txt
```

??? note "Alternative : conda"

    Si vous utilisez déjà [conda](https://conda-forge.org/) (Miniforge,
    Anaconda), vous pouvez tout installer, y compris MPI, avec une seule
    commande, sous Linux comme sous macOS :

    ```bash
    conda create -n hpc -c conda-forge python numpy matplotlib mpi4py openmpi
    conda activate hpc
    ```

## 4. Vérifier l'installation

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

## 5. Mettre à jour le dépôt

Le cours évolue pendant l'année. Pour récupérer les dernières versions,
depuis le dossier du dépôt :

```bash
git pull
```

!!! warning "Travaillez dans vos propres fichiers"

    Si vous modifiez les fichiers du dépôt, `git pull` peut entrer en
    conflit avec les mises à jour. Le plus simple est de copier un
    exercice avant de le modifier, ou de travailler dans un dossier à part.

## 6. Éditer le code avec VS Code

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

??? question "Sous macOS, un message `shmem: mmap: an error occurred`"

    Cet avertissement d'Open MPI est sans conséquence : le programme
    s'exécute normalement. Pour le faire disparaître, lancez avant
    `mpirun` :

    ```bash
    export TMPDIR=/tmp
    ```
