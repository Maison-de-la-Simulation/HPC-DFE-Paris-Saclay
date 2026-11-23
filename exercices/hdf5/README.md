# Exercice : écriture parallèle de fichiers HDF5

Dans cet exercice, plusieurs processus MPI possèdent chacun une partie d'un
tableau global. Le code fourni est déjà parallélisé avec MPI, mais il ne
sauvegarde rien : c'est à vous d'écrire ce tableau dans des fichiers HDF5.
Vous allez implémenter et comparer, **dans le même fichier `main.py`**, les
trois méthodes vues dans le cours :

1. **un fichier par rang** ;
2. **tout ramener sur le rang 0**, qui écrit seul le fichier ;
3. **l'écriture HDF5 parallèle**, où tous les rangs écrivent dans le même
   fichier.

Cours associé : [Écriture parallèle de fichiers avec HDF5](https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/hdf5/index.html)

## Le problème

Un tableau global de `Nx` × `Ny` réels est découpé en **bandes de lignes**
contiguës, une par rang. Chaque rang remplit sa bande avec **son numéro de
rang**. Quand `Nx` n'est pas divisible par le nombre de rangs, les premiers
rangs reçoivent une ligne de plus.

Par exemple, avec `Nx = 13` et 4 rangs, les rangs 0 à 3 possèdent
respectivement 4, 3, 3 et 3 lignes :

```text
lignes  0 à  3 : 0 0 0 0 0 0 0 0
lignes  4 à  6 : 1 1 1 1 1 1 1 1
lignes  7 à  9 : 2 2 2 2 2 2 2 2
lignes 10 à 12 : 3 3 3 3 3 3 3 3
```

La fonction `block_rows(Nx, number_of_ranks, rank)`, fournie, renvoie la
première ligne `start` et le nombre de lignes `n_local` d'un rang.

## Fichiers fournis

| Fichier | Rôle |
|---|---|
| `main.py` | Code de départ, **déjà en MPI** : chaque rang construit sa bande du tableau. Il contient trois sections, une par méthode d'écriture, à compléter |
| `check.py` | Vérifie le ou les fichiers produits et affiche le tableau reconstruit avec matplotlib |
| `correction/main.py` | La correction des trois méthodes |

## Prérequis

- Les méthodes 1 et 2 nécessitent `numpy`, `mpi4py` et `h5py` (la version
  standard, installée avec `pip install h5py`).
- La méthode 3 nécessite **h5py compilé avec MPI**. Vérifiez-le avec :

  ```bash
  python -c "import h5py; print(h5py.get_config().mpi)"
  ```

  Si la commande affiche `False`, voir la section *Installer h5py en version
  parallèle* du cours, ou réalisez la méthode 3 sur Ruche.

Sur Ruche, chargez l'environnement du cours avant de commencer :

```bash
source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh
```

## Consignes

### 0. Prendre en main le code de départ

1. Lisez `main.py` : repérez le découpage du tableau en bandes, la bande
   `local` de chaque rang, et les trois sections `Méthode 1`, `Méthode 2`
   et `Méthode 3` à compléter.

2. Exécutez-le :

   ```bash
   mpirun -np 4 python main.py --Nx 13 --Ny 8
   ```

   Chaque rang affiche les lignes qu'il possède, puis le temps de chaque
   méthode s'affiche. Aucun fichier n'est encore produit.

Chaque méthode écrit dans des fichiers différents, ce qui permet de les
vérifier séparément avec `check.py`, qui affiche aussi le tableau
reconstruit. Sans écran (par exemple sur Ruche sans affichage graphique),
ajoutez l'option `--save champ.png` pour enregistrer l'image au lieu de
l'afficher. Les outils HDF5 permettent aussi
d'explorer un fichier :

```bash
h5ls -r methode2.h5
h5dump methode2.h5
```

### 1. Méthode 1 : un fichier par rang

Complétez la section `Méthode 1` de `main.py`.

1. Chaque rang écrit sa bande dans **son propre fichier**
   `methode1_rang_<rang>.h5`, dans un dataset `champ`.

2. Pour que l'on puisse reconstruire le tableau global, ajoutez au dataset
   les attributs :
   - `start` : la première ligne de la bande ;
   - `global_shape` : la taille du tableau global `(Nx, Ny)` ;
   - `number_of_ranks` : le nombre de rangs.

3. Exécutez et vérifiez :

   ```bash
   mpirun -np 4 python main.py --Nx 13 --Ny 8
   python check.py methode1_rang_*.h5
   ```

### 2. Méthode 2 : tout ramener sur le rang 0

Complétez la section `Méthode 2` de `main.py`.

1. Rassemblez toutes les bandes sur le rang 0. Les bandes n'ont pas toutes
   la même taille : utilisez `comm.Gatherv`. Il faut lui donner, pour chaque
   rang :
   - `count` : le nombre de valeurs envoyées, soit `n_local * Ny` ;
   - `displacement` : la position de la première valeur dans le tableau
     global vu comme un tableau 1D, soit `start * Ny`.

2. Le rang 0, et lui seul, écrit le tableau global dans `methode2.h5`,
   dans un dataset `champ` avec l'attribut `number_of_ranks`.

3. Exécutez et vérifiez :

   ```bash
   mpirun -np 4 python main.py --Nx 13 --Ny 8
   python check.py methode2.h5
   ```

### 3. Méthode 3 : écriture HDF5 parallèle

Complétez la section `Méthode 3` de `main.py`.

1. Tous les rangs ouvrent **le même fichier** `methode3.h5` avec le pilote
   `mpio` :

   ```python
   h5py.File("methode3.h5", "w", driver="mpio", comm=comm)
   ```

2. Tous les rangs créent le dataset `champ` avec la taille **globale**
   `(Nx, Ny)` et l'attribut `number_of_ranks`. Ces opérations sont
   collectives : tous les rangs doivent les appeler avec les mêmes
   arguments.

3. Chaque rang écrit sa bande aux lignes `start` à `start + n_local`.
   Faites-le en écriture **collective**, avec `with dset.collective:`.

4. Exécutez et vérifiez :

   ```bash
   mpirun -np 4 python main.py --Nx 13 --Ny 8
   python check.py methode3.h5
   ```

### 4. Comparer les trois méthodes

1. Le code affiche, pour chaque méthode, le temps d'écriture du rang le
   plus lent.

2. Sur Ruche, comparez les trois méthodes pour un gros tableau (par
   exemple `--Nx 20000 --Ny 20000`, soit 3,2 Go) et pour 4, 16 puis 40
   processus.

3. Commentez :
   - Combien de fichiers chaque méthode produit-elle ? Lesquels sont
     faciles à exploiter ensuite ?
   - Quelle quantité de mémoire le rang 0 doit-il avoir dans la méthode 2 ?
     Que se passe-t-il si le tableau global ne tient pas dans la mémoire
     d'un nœud ?
   - Quelle méthode vous semble la plus adaptée à une simulation sur des
     milliers de processus ?
