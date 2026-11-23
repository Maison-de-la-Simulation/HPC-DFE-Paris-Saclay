# IV. Écriture parallèle de fichiers avec HDF5

Une simulation parallèle produit des résultats **répartis** entre les
processus : chaque rang ne possède que sa partie du domaine. Ce chapitre
montre comment écrire ces résultats dans **un seul fichier**, en
parallèle, avec le format HDF5.

Il s'appuie sur le chapitre [MPI](../mpi/index.md), en particulier sur
la répartition du travail entre les rangs et sur la topologie
cartésienne.

## Trois façons d'écrire des données réparties

--8<-- "hdf5/strategies-ecriture.svg"

| Stratégie | Avantages | Inconvénients |
|---|---|---|
| **1. Un fichier par processus** | Très simple, aucune communication | Autant de fichiers que de processus, qu'il faut recombiner ; ingérable avec des milliers de processus |
| **2. Tout rassembler sur le rang 0** | Un seul fichier, écrit en séquentiel | Le rang 0 doit stocker **toutes** les données ; il écrit seul pendant que les autres attendent |
| **3. Écriture parallèle** | Un seul fichier, chaque rang écrit sa partie **en même temps** | Nécessite une bibliothèque d'entrées-sorties parallèles |

Sur un super-calculateur, la troisième stratégie est la seule qui passe à
l'échelle. Elle repose sur **MPI-IO**, la partie de MPI dédiée aux
fichiers. HDF5 l'utilise en coulisse et offre une interface beaucoup plus
simple.

Vous mettrez en œuvre et comparerez ces trois méthodes dans
l'[exercice](#exercice-les-trois-methodes-decriture) de fin de chapitre.

## Le format HDF5

[HDF5](https://www.hdfgroup.org/solutions/hdf5/) (*Hierarchical Data
Format*) est un format de fichier binaire très utilisé en calcul
scientifique. Un fichier HDF5 s'organise comme un système de fichiers :

Groupe
:   L'équivalent d'un dossier. Il contient des datasets et d'autres
    groupes. Le groupe racine s'appelle `/`.

Dataset
:   Un tableau multidimensionnel de données (réels, entiers…), avec une
    forme et un type, comme un tableau NumPy.

Attribut
:   Une petite métadonnée attachée à un groupe ou à un dataset : une
    unité, un pas de temps, une description.

Par exemple, un fichier de résultats de simulation peut ranger le
maillage dans un groupe et chaque instant sauvegardé dans un autre :

--8<-- "hdf5/structure-fichier.svg"

Chaque élément est désigné par son **chemin**, comme un fichier dans un
dossier : le dataset de température est `/temps_0010/temperature`.

Les fichiers HDF5 sont **portables** (lisibles quel que soit
l'ordinateur), **auto-descriptifs** (on peut explorer leur contenu sans
connaître le code qui les a écrits) et lisibles par de nombreux outils :
h5py, ParaView, VisIt, Matlab…

En Python, on utilise la bibliothèque [h5py](https://docs.h5py.org/),
dont les datasets s'utilisent comme des tableaux NumPy :

```python
import h5py
import numpy as np

# Écriture (en séquentiel)
with h5py.File("resultats.h5", "w") as f:
    f.create_dataset("temperature", data=np.linspace(0.0, 1.0, 10))
    f["temperature"].attrs["unite"] = "K"

# Lecture
with h5py.File("resultats.h5", "r") as f:
    T = f["temperature"][...]           # tableau NumPy
    print(f["temperature"].attrs["unite"])
```

## Écrire en parallèle avec h5py

Pour écrire en parallèle, on ouvre le fichier avec le pilote **`mpio`** et
le communicateur MPI. Tous les processus ouvrent alors **le même
fichier** :

```python
f = h5py.File("resultats.h5", "w", driver="mpio", comm=MPI.COMM_WORLD)
```

!!! warning "Il faut une version parallèle de h5py"

    La version de h5py installée avec `pip install h5py` ne gère pas MPI.
    On le vérifie avec :

    ```python
    import h5py
    print(h5py.get_config().mpi)        # doit afficher True
    ```

    Sinon, l'ouverture du fichier échoue avec le message
    `h5py was built without MPI support, can't use mpio driver`. Voir
    [Installer h5py en version parallèle](#installer-h5py-en-version-parallele).

### Les règles à respecter

Opérations **collectives**
:   Tout ce qui modifie la **structure** du fichier doit être appelé par
    **tous les processus**, avec **les mêmes arguments** : ouvrir et
    fermer le fichier, créer un groupe ou un dataset, écrire un attribut.

Écriture des **données**
:   Chaque processus écrit ensuite **sa partie** du dataset, en indiquant
    les indices concernés. Ces écritures peuvent être différentes d'un
    processus à l'autre.

En pratique : on crée le dataset avec sa taille **globale** sur tous les
rangs, puis chaque rang remplit sa tranche.

### Exemple 1D : chaque rang écrit sa tranche

On reprend la répartition d'un tableau de `N` valeurs en blocs contigus,
vue dans la partie [Premier programme parallèle](../mpi/premier-programme.md#repartir-le-travail-entre-les-rangs) :

```python
import h5py
import numpy as np
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
number_of_ranks = comm.Get_size()

N = 10

# Bloc de ce rang (les premiers rangs prennent le reste)
n_local = N // number_of_ranks + (1 if rank < N % number_of_ranks else 0)
start = rank * (N // number_of_ranks) + min(rank, N % number_of_ranks)

# Données locales : ici, chaque rang écrit son numéro
local = np.full(n_local, float(rank))

with h5py.File("resultats.h5", "w", driver="mpio", comm=comm) as f:
    # Collectif : tous les rangs créent le même dataset, de taille globale
    dset = f.create_dataset("x", (N,), dtype="f8")
    dset.attrs["description"] = "numéro du rang qui a écrit chaque valeur"

    # Chaque rang écrit sa tranche
    dset[start:start + n_local] = local
```

Avec 4 processus, le dataset `x` contient :

```text
[0. 0. 0. 1. 1. 1. 2. 2. 3. 3.]
```

### Exemple 2D : avec une topologie cartésienne

Avec une [topologie cartésienne](../mpi/topologie.md), chaque rang possède un
bloc de la grille globale. Ses coordonnées indiquent où écrire ce bloc :

```python
# Grille de 2 × 2 processus, blocs locaux de 3 × 4 valeurs
dims = [2, 2]
cart_comm = comm.Create_cart(dims, periods=[False, False], reorder=True)
cart_rank = cart_comm.Get_rank()
c0, c1 = cart_comm.Get_coords(cart_rank)

nx, ny = 3, 4
local = np.full((nx, ny), float(cart_rank))

with h5py.File("champ.h5", "w", driver="mpio", comm=cart_comm) as f:
    dset = f.create_dataset("champ", (dims[0] * nx, dims[1] * ny), dtype="f8")
    dset[c0 * nx:(c0 + 1) * nx, c1 * ny:(c1 + 1) * ny] = local
```

Le dataset `champ` (6 × 8) contient alors les 4 blocs à leur place :

```text
[[0. 0. 0. 0. 1. 1. 1. 1.]
 [0. 0. 0. 0. 1. 1. 1. 1.]
 [0. 0. 0. 0. 1. 1. 1. 1.]
 [2. 2. 2. 2. 3. 3. 3. 3.]
 [2. 2. 2. 2. 3. 3. 3. 3.]
 [2. 2. 2. 2. 3. 3. 3. 3.]]
```

## Écritures indépendantes ou collectives

### Pourquoi écrire collectivement ?

Un fichier HDF5 stocke un tableau 2D **ligne par ligne**. Le bloc d'un
rang, qui est un rectangle dans la grille, se retrouve donc découpé en
plusieurs morceaux dans le fichier : un morceau par ligne du bloc.

--8<-- "hdf5/ecriture-collective.svg"

Écriture indépendante (par défaut)
:   Chaque processus écrit seul ses morceaux, sans se coordonner avec les
    autres. Avec de gros blocs de plusieurs centaines de lignes, cela fait
    un très grand nombre de **petites écritures**, dispersées dans le
    fichier. C'est lent, surtout sur les systèmes de fichiers parallèles
    des super-calculateurs, faits pour de gros accès contigus.

Écriture collective
:   Tous les processus écrivent **ensemble**. MPI-IO regroupe d'abord les
    données entre processus, en général sur quelques processus
    **agrégateurs**, qui écrivent ensuite de **gros morceaux contigus**.
    Il y a un peu de communication en plus, mais beaucoup moins d'accès
    au disque.

En h5py, on active l'écriture collective avec le bloc
`with dset.collective:` :

```python
with dset.collective:
    dset[start:start + n_local] = local
```

### Exemple : la topologie cartésienne en écriture collective

On reprend l'[exemple 2D avec une topologie cartésienne](#exemple-2d-avec-une-topologie-cartesienne).
La seule différence est que l'écriture des données se fait maintenant dans
un bloc `with dset.collective:` :

```python
# Grille de 2 × 2 processus, blocs locaux de 3 × 4 valeurs
dims = [2, 2]
cart_comm = comm.Create_cart(dims, periods=[False, False], reorder=True)
cart_rank = cart_comm.Get_rank()
c0, c1 = cart_comm.Get_coords(cart_rank)

nx, ny = 3, 4
local = np.full((nx, ny), float(cart_rank))

with h5py.File("champ.h5", "w", driver="mpio", comm=cart_comm) as f:
    dset = f.create_dataset("champ", (dims[0] * nx, dims[1] * ny), dtype="f8")

    # Écriture collective : tous les rangs écrivent leur bloc ensemble
    with dset.collective:
        dset[c0 * nx:(c0 + 1) * nx, c1 * ny:(c1 + 1) * ny] = local
```

Le contenu du fichier est **le même** qu'en écriture indépendante : seule
la façon dont MPI-IO accède au disque change.

```text
[[0. 0. 0. 0. 1. 1. 1. 1.]
 [0. 0. 0. 0. 1. 1. 1. 1.]
 [0. 0. 0. 0. 1. 1. 1. 1.]
 [2. 2. 2. 2. 3. 3. 3. 3.]
 [2. 2. 2. 2. 3. 3. 3. 3.]
 [2. 2. 2. 2. 3. 3. 3. 3.]]
```

Chaque bloc occupe ici 3 lignes du fichier : en écriture indépendante,
chaque rang ferait 3 petites écritures de 4 valeurs ; en écriture
collective, MPI-IO peut les regrouper en quelques gros accès contigus.

!!! warning "Les règles de l'écriture collective"

    - **Tous les processus** doivent exécuter le bloc
      `with dset.collective:`, sinon le programme reste bloqué : comme
      pour toute communication collective, chacun attend les autres.
    - La création des datasets et des attributs est **toujours**
      collective, même pour une écriture indépendante : tous les rangs les
      créent, avec les mêmes arguments (ici `create_dataset`).
    - Le fichier est fermé par tous les processus à la sortie du bloc
      `with h5py.File(...)`.

!!! tip "Indépendante ou collective ?"

    Pour de petits fichiers ou quelques processus, la différence est
    faible. Sur un super-calculateur, avec beaucoup de processus et de
    gros volumes, l'écriture collective est en général nettement plus
    rapide : c'est le choix par défaut à privilégier.

## Relire et visualiser le fichier

Le fichier produit est un fichier HDF5 ordinaire : on le relit en
séquentiel, sur un seul processus, sans MPI.

=== "Python"

    ```python
    import h5py

    with h5py.File("champ.h5", "r") as f:
        print(list(f.keys()))           # ['champ']
        champ = f["champ"][...]         # tableau NumPy 6 × 8
    ```

=== "Ligne de commande"

    Les outils fournis avec HDF5 affichent la structure et le contenu
    d'un fichier :

    ```bash
    h5ls -r champ.h5        # liste des groupes et datasets
    h5dump champ.h5         # contenu complet
    ```

=== "ParaView"

    [ParaView](https://www.paraview.org/) sait ouvrir les fichiers HDF5,
    en particulier lorsqu'ils sont accompagnés d'un petit fichier de
    description XDMF qui indique comment interpréter les datasets.

## Installer h5py en version parallèle

La version parallèle de h5py doit être compilée avec une bibliothèque
HDF5 parallèle, elle-même compilée avec MPI : pip ne la fournit pas. La
procédure pour Linux, macOS, Windows (WSL) et conda est décrite dans le
guide d'installation :

:material-laptop: [Installer HDF5 en version parallèle](../installation.md#4-installer-hdf5-en-version-parallele-facultatif)

Vérifiez le résultat avec :

```bash
python -c "import h5py; print(h5py.get_config().mpi)"   # doit afficher True
```

## Exercice : les trois méthodes d'écriture

Cet exercice met en pratique les trois façons d'écrire des données
réparties vues au début de ce chapitre.

**Le problème.** Un tableau global de `Nx` × `Ny` réels est découpé en
bandes de lignes contiguës, une par rang MPI. Chaque rang ne possède que
sa bande, qu'il remplit avec son numéro de rang. Avec `Nx = 13` et 4
rangs, les rangs 0 à 3 possèdent respectivement 4, 3, 3 et 3 lignes.

**Le code de départ.** Le fichier `main.py` est déjà parallélisé avec
MPI : chaque rang construit sa bande, mais rien n'est sauvegardé. Il
contient trois sections à compléter, une par méthode, chacune chronométrée
avec `MPI.Wtime` :

| Méthode | À implémenter | Fichiers produits |
|---|---|---|
| 1. Un fichier par rang | Chaque rang écrit sa bande, avec sa position en attributs | `methode1_rang_<rang>.h5` |
| 2. Tout sur le rang 0 | `Gatherv` des bandes, puis écriture par le rang 0 seul | `methode2.h5` |
| 3. HDF5 parallèle | Pilote `mpio`, dataset global, écriture collective | `methode3.h5` |

**La vérification.** Le script `check.py` reconstruit le tableau à partir
d'un fichier unique ou d'un fichier par rang, vérifie que chaque bande
contient le bon numéro de rang, puis affiche le tableau avec matplotlib :

```bash
mpirun -np 4 python main.py --Nx 13 --Ny 8

python check.py methode1_rang_*.h5
python check.py methode2.h5
python check.py methode3.h5
```

Sans écran, l'option `--save champ.png` enregistre l'image au lieu de
l'afficher.

**La comparaison.** Sur Ruche, comparez les temps des trois méthodes pour
un gros tableau et un nombre croissant de processus, puis discutez du
nombre de fichiers produits, de la mémoire nécessaire au rang 0 et du
passage à l'échelle.

!!! example "Exercice — Écriture parallèle de fichiers HDF5"

    :material-file-document: [Énoncé détaillé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/hdf5)

    Dans votre éditeur : `exercices/hdf5/README.md`.

    La méthode 3 nécessite h5py compilé avec MPI (voir la section
    précédente). Sans lui, réalisez les méthodes 1 et 2 sur votre
    ordinateur et la méthode 3 sur Ruche.

## Bilan

!!! success "À ce stade, vous savez"

    - choisir une stratégie pour écrire des données réparties ;
    - décrire la structure d'un fichier HDF5 : groupes, datasets,
      attributs ;
    - écrire en parallèle dans un seul fichier HDF5 avec h5py ;
    - relire et explorer le fichier produit ;
    - comparer les trois méthodes d'écriture sur un cas concret.
