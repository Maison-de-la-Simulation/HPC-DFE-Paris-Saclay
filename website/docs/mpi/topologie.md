# 5. Topologie cartésienne

## Décomposition de domaine

En calcul scientifique, il est courant de **décomposer le domaine
d'étude** (grille, matrice) en **sous-domaines**, chacun géré par un
processus MPI unique.

Sur une grille régulière et structurée, une approche simple et classique
consiste à diviser le domaine en **blocs réguliers** : chaque processus
possède dans sa mémoire locale une partie unique de la grille globale.
C'est la **décomposition cartésienne**, un exemple typique de
**parallélisme de données**.

--8<-- "mpi/decomposition.svg"

## Coordonnées et rangs

Une topologie cartésienne a besoin :

- de **coordonnées** pour situer chaque processus (chaque bloc) dans
  l'espace cartésien ;
- de **rangs** attribués en cohérence avec ces coordonnées.

Pour une grille de 3 × 4 processus, `Get_coords` renvoie les coordonnées
dans l'ordre des directions : (direction 0, direction 1). Par exemple, le
rang 5 a pour coordonnées (1, 1) et le rang 6 (1, 2).

--8<-- "mpi/coords.svg"

Il y a deux façons de mettre en place une topologie cartésienne : la
construire à la main, ou faire appel aux **fonctions MPI** prévues pour
cela, présentées ci-dessous.

## Créer la topologie : `Create_cart`

`Create_cart` crée un nouveau communicateur doté d'une topologie
cartésienne, à partir d'un communicateur existant (par exemple
`MPI.COMM_WORLD`) :

```python
cart_comm = comm.Create_cart(dims, periods, reorder=True)
```

| Paramètre | Rôle |
|---|---|
| `comm` | Communicateur à partir duquel on construit la topologie |
| `dims` | Liste du nombre de processus dans chaque direction ; sa longueur donne la dimension de la topologie |
| `periods` | Liste de booléens : chaque direction est-elle périodique (`True`) ou non (`False`) ? |
| `reorder` | Autorise MPI à renuméroter les rangs pour optimiser les échanges |
| `cart_comm` | Nouveau communicateur, doté de la topologie cartésienne |

Comme pour tout communicateur, on récupère son rang dans `cart_comm` avec
`Get_rank()`. Avec `reorder=True`, il peut différer du rang dans
`MPI.COMM_WORLD` : on utilise donc toujours `cart_comm` ensuite.

## Récupérer les coordonnées : `Get_coords`

```python
cart_coords = cart_comm.Get_coords(rank)
```

`Get_coords` renvoie les coordonnées du rang `rank` dans la topologie
cartésienne `cart_comm`.

## Les voisins

Chaque processus doit pouvoir récupérer le rang de ses **voisins** dans
la topologie, pour communiquer avec eux (par exemple pour échanger les
bords de son sous-domaine).

--8<-- "mpi/neighbours.svg"

### `Shift`

`Shift` renvoie les rangs voisins le long d'une direction donnée. Comme
pour `sendrecv`, on récupère **deux** voisins : un rang **source** et un
rang **destinataire**.

```python
source_rank, dest_rank = cart_comm.Shift(direction, displacement)
```

| Paramètre | Rôle |
|---|---|
| `direction` (`int`) | 0 pour la première coordonnée, 1 pour la deuxième, etc. |
| `displacement` (`int`) | Pas de déplacement : si > 0, vers les coordonnées croissantes ; si < 0, vers les coordonnées décroissantes |
| `source_rank` | Avec `displacement > 0`, le voisin du côté des coordonnées **décroissantes** (à « gauche ») |
| `dest_rank` | Avec `displacement > 0`, le voisin du côté des coordonnées **croissantes** (à « droite ») |

--8<-- "mpi/shift.svg"

### Bords et périodicité : `MPI.PROC_NULL`

Lorsqu'un rang n'a pas de voisin dans une direction (sur un bord, en
non périodique), `Shift` renvoie `MPI.PROC_NULL`.

- Une communication dont l'expéditeur ou le destinataire est
  `MPI.PROC_NULL` peut être écrite normalement : elle est simplement
  **ignorée**. On évite ainsi de multiplier les conditions sur les bords.
- Tester si un voisin vaut `MPI.PROC_NULL` revient à tester si le rang
  est sur un bord de la topologie (en non périodique).

```python
left_rank = cart_comm.Shift(0, 1)[0]
if left_rank == MPI.PROC_NULL:
    # Ce rang est sur le bord, par définition
    ...
```

Dans une direction **périodique**, les bords se rejoignent : sur la
grille 3 × 4 ci-dessus, si la direction 1 est périodique, le voisin en −x
du rang 4 est le rang 7 au lieu de `MPI.PROC_NULL`.

## Exemple : topologie cartésienne 3D

```python
# Nombre de processus par direction : 4 × 5 × 2 = 40 processus
dims = [4, 5, 2]

# Seule la troisième direction est périodique
periods = [False, False, True]

# Création du communicateur cartésien
cart_comm = comm.Create_cart(dims, periods, reorder=True)

# Rang et coordonnées dans le communicateur cartésien
cart_rank = cart_comm.Get_rank()
cart_coords = cart_comm.Get_coords(cart_rank)

# Voisins dans la direction 0, avec un déplacement de +1
left_rank, right_rank = cart_comm.Shift(0, 1)
```

!!! example "Exercice 9 — Topologie cartésienne"

    Mettre en place une topologie cartésienne 2D.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/9_cartesian_topology)

    Dans votre éditeur :
    `exercices/mpi/python/9_cartesian_topology/README.md`.

## Bilan

!!! success "À ce stade, vous savez"

    - créer un communicateur cartésien pour décomposer vos données ;
    - utiliser les fonctions liées à la décomposition cartésienne.
