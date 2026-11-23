# 5.1 Communications collectives : diffusion et collecte

## Les différents types

Les communications **collectives** font intervenir plusieurs processus,
voire tout le communicateur, pour propager ou rassembler de
l'information. Il en existe trois types :

- la **synchronisation** : c'est la barrière `Barrier` ;
- le **transfert de données** : diffusion, collecte ;
- le **transfert avec opération** sur les données : les **réductions**.

Tous les processus du communicateur doivent appeler la fonction
collective. Les versions standard induisent une barrière implicite pour
les processus concernés : pour chacun, elle est relâchée dès que sa
participation est terminée.

## Diffusion générale : `bcast`

Envoi d'une donnée depuis un processus vers **tous** les processus du
communicateur.

--8<-- "mpi/bcast.svg"

```python
data = comm.bcast(data, root=0)
```

| Paramètre | Rôle |
|---|---|
| `data` | Sur `root` : la donnée à diffuser. Sur les autres rangs : ignorée (on passe en général `None`) |
| `root` | Rang qui possède la donnée et la diffuse |

`bcast` est appelée par **tous** les processus et renvoie, sur chacun, une
copie de la donnée de `root`.

### Exemples de diffusion

=== "Un paramètre"

    Le rang 0 choisit un paramètre (lu dans un fichier ou sur la ligne de
    commande, par exemple) et le transmet à tous les autres :

    ```python
    if rank == 0:
        n = 1000
    else:
        n = None

    n = comm.bcast(n, root=0)
    print(f"Rang {rank} : n = {n}")
    ```

    Chaque rang affiche `n = 1000`.

=== "Un dictionnaire de paramètres"

    `bcast` accepte n'importe quel objet Python. C'est pratique pour
    diffuser en une seule fois tous les paramètres d'une simulation :

    ```python
    if rank == 0:
        params = {"Nx": 256, "Ny": 128, "dt": 1e-3, "nom": "kelvin-helmholtz"}
    else:
        params = None

    params = comm.bcast(params, root=0)
    print(f"Rang {rank} : Nx = {params['Nx']}, dt = {params['dt']}")
    ```

=== "Un tableau NumPy"

    Pour un tableau NumPy, on utilise la version à majuscule `Bcast`, plus
    rapide. Elle ne renvoie rien : elle **remplit** un tableau qui doit
    déjà exister, avec la même taille et le même type, sur tous les rangs.

    ```python
    import numpy as np

    # Le tableau doit exister sur tous les rangs
    grid = np.empty(5, dtype=np.float64)

    if rank == 0:
        grid[:] = np.linspace(0.0, 1.0, 5)

    comm.Bcast(grid, root=0)
    print(f"Rang {rank} : {grid}")
    ```

    Chaque rang affiche `[0.   0.25 0.5  0.75 1.  ]`.

=== "Depuis un autre rang"

    Le rang qui diffuse n'est pas forcément le rang 0 : c'est `root` qui
    le désigne.

    ```python
    if rank == 2:
        message = f"bonjour de la part du rang {rank}"
    else:
        message = None

    message = comm.bcast(message, root=2)
    ```

    Tous les rangs reçoivent `"bonjour de la part du rang 2"`.

!!! warning "Une erreur fréquente"

    Mettre l'appel à `bcast` dans un `if rank == 0:` : les autres rangs
    n'appellent jamais la fonction et le programme reste bloqué. Comme
    toute collective, `bcast` doit être appelée par **tous** les
    processus ; seule la préparation de la donnée se fait sur `root`.

## Diffusion sélective : `scatter`

Partage de données, choisi par le développeur, depuis un processus vers
tous les processus du communicateur : chacun reçoit **sa part**.

--8<-- "mpi/scatter.svg"

```python
# Sur le rang 0, data est une liste d'autant d'éléments que de rangs
part = comm.scatter(data, root=0)
```

| Paramètre | Rôle |
|---|---|
| `data` | Sur `root` : une liste d'**exactement autant d'éléments que de rangs**. Sur les autres rangs : ignorée (on passe en général `None`) |
| `root` | Rang qui possède les données et les distribue |
| `part` | L'élément reçu : le rang `i` reçoit `data[i]` |

`scatter` est appelée par **tous** les processus. Contrairement à `bcast`,
qui envoie **la même** donnée à tous, `scatter` envoie à chaque rang **une
donnée différente**.

### Exemples de distribution

=== "Une liste de nombres"

    Le rang 0 prépare une valeur par rang et chaque rang reçoit la sienne :

    ```python
    size = comm.Get_size()

    if rank == 0:
        data = [10 * i for i in range(size)]
    else:
        data = None

    part = comm.scatter(data, root=0)
    print(f"Rang {rank} : part = {part}")
    ```

    Avec 4 processus, les rangs 0, 1, 2 et 3 reçoivent 0, 10, 20 et 30.

=== "Une tâche par rang"

    Les éléments de la liste peuvent être n'importe quels objets Python,
    par exemple la description du travail de chaque rang :

    ```python
    size = comm.Get_size()

    if rank == 0:
        taches = [{"fichier": f"resultat_{i}.dat", "seed": 100 + i}
                  for i in range(size)]
    else:
        taches = None

    tache = comm.scatter(taches, root=0)
    print(f"Rang {rank} : {tache}")
    ```

    Le rang 1 reçoit `{'fichier': 'resultat_1.dat', 'seed': 101}`.

=== "Découper un tableau NumPy"

    Pour un tableau NumPy, on utilise la version à majuscule `Scatter`.
    Le tableau complet, sur `root`, est découpé en **morceaux de même
    taille**, dans l'ordre des rangs. Chaque rang doit avoir préparé un
    tableau pour recevoir son morceau.

    ```python
    import numpy as np

    size = comm.Get_size()

    n_local = 3
    local = np.empty(n_local, dtype=np.float64)   # sur tous les rangs

    if rank == 0:
        full = np.arange(n_local * size, dtype=np.float64)
    else:
        full = None

    comm.Scatter(full, local, root=0)
    print(f"Rang {rank} : {local}")
    ```

    Avec 4 processus, le tableau `[0, 1, …, 11]` est découpé en
    `[0, 1, 2]`, `[3, 4, 5]`, `[6, 7, 8]` et `[9, 10, 11]`.

=== "Des parts de tailles différentes"

    Avec `scatter` (minuscule), chaque élément de la liste peut être
    lui-même une liste, de longueur quelconque. On peut ainsi répartir 10
    valeurs sur 4 rangs :

    ```python
    size = comm.Get_size()

    if rank == 0:
        valeurs = list(range(10))
        # valeurs   = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        morceaux = [valeurs[i::size] for i in range(size)]
        # morceaux  = [[0, 4, 8], [1, 5, 9], [2, 6], [3, 7]]
    else:
        morceaux = None

    mes_valeurs = comm.scatter(morceaux, root=0)
    ```

    Sur le rang 0, avec 4 processus :

    - `valeurs` contient les 10 entiers de 0 à 9 ;
    - `morceaux` est une liste de 4 listes, une par rang. `valeurs[i::size]`
      prend une valeur sur 4 à partir de l'indice `i` : le morceau 0
      contient `[0, 4, 8]`, le morceau 1 `[1, 5, 9]`, le morceau 2
      `[2, 6]` et le morceau 3 `[3, 7]`.

    `scatter` envoie ensuite le morceau `i` au rang `i` : les rangs 0, 1,
    2 et 3 reçoivent `[0, 4, 8]`, `[1, 5, 9]`, `[2, 6]` et `[3, 7]` dans
    `mes_valeurs`.
    Pour des tableaux NumPy de tailles différentes, il faut utiliser
    `Scatterv`, l'équivalent de
    [`Gatherv`](collectives-avancees.md#collecte-de-tailles-variables-gatherv).

!!! warning "Une erreur fréquente"

    Sur `root`, la liste doit contenir **exactement un élément par rang**.
    Avec une liste plus courte ou plus longue, mpi4py lève une erreur
    sur `root` (par exemple `ValueError: expecting 4 items, got 3`).
    Pour répartir plus de données que de rangs, on regroupe d'abord les
    données en autant de morceaux que de rangs, comme dans le dernier
    exemple.

## Collecte : `gather`

Envoi de données réparties sur plusieurs processus vers **un processus
unique**, qui les rassemble dans l'ordre des rangs.

--8<-- "mpi/gather.svg"

`gather` est appelée par tous les processus en même temps :

=== "Objets Python"

    ```python
    # Sur le rang 0 : liste des valeurs de tous les rangs ; ailleurs : None
    values = comm.gather(local_value, root=0)
    ```

=== "Tableaux NumPy"

    ```python
    comm.Gather(send_buffer, recv_buffer, root=0)
    ```

    | Paramètre | Rôle |
    |---|---|
    | `send_buffer` | Valeur ou ensemble de valeurs envoyé par chaque processus |
    | `recv_buffer` | Tableau réunissant les valeurs reçues (utilisé sur `root` seulement) |
    | `root` | Processus qui reçoit les données et reconstruit le tableau final |

`gather` est l'opération inverse de `scatter` : chaque rang envoie **une**
donnée, et `root` reçoit la liste de toutes les données, rangées dans
l'ordre des rangs (la donnée du rang `i` est à l'indice `i`).

### Exemples de collecte

=== "Une valeur par rang"

    Chaque rang calcule une valeur, et le rang 0 les récupère toutes :

    ```python
    local_value = rank ** 2
    # rang 0 : 0, rang 1 : 1, rang 2 : 4, rang 3 : 9

    values = comm.gather(local_value, root=0)
    print(f"Rang {rank} : values = {values}")
    ```

    Avec 4 processus :

    - sur le rang 0, `values` vaut `[0, 1, 4, 9]` : l'élément `i` est la
      valeur envoyée par le rang `i` ;
    - sur les rangs 1, 2 et 3, `values` vaut `None`.

=== "Rassembler des résultats"

    Les données collectées peuvent être n'importe quels objets Python, par
    exemple un résumé du travail de chaque rang, que le rang 0 affiche :

    ```python
    infos = {"rang": rank, "temps": round(0.5 + 0.1 * rank, 2)}
    # rang 1 : infos = {"rang": 1, "temps": 0.6}

    toutes = comm.gather(infos, root=0)

    if rank == 0:
        for d in toutes:
            print(f"Rang {d['rang']} : {d['temps']} s")
    ```

    Sur le rang 0, `toutes` est la liste des 4 dictionnaires :
    `[{'rang': 0, 'temps': 0.5}, {'rang': 1, 'temps': 0.6}, …]`. Le rang 0
    affiche une ligne par rang.

=== "Reconstruire un tableau NumPy"

    Pour des tableaux NumPy, on utilise la version à majuscule `Gather`.
    Chaque rang envoie un morceau de **même taille**, et `root` doit
    avoir préparé un tableau assez grand pour tous les recevoir.

    ```python
    import numpy as np

    size = comm.Get_size()

    n_local = 3
    local = np.full(n_local, float(rank))
    # rang 0 : [0. 0. 0.], rang 1 : [1. 1. 1.], ...

    if rank == 0:
        full = np.empty(n_local * size, dtype=np.float64)   # 12 valeurs
    else:
        full = None

    comm.Gather(local, full, root=0)
    ```

    Avec 4 processus, sur le rang 0, `full` vaut
    `[0. 0. 0. 1. 1. 1. 2. 2. 2. 3. 3. 3.]` : les morceaux sont placés
    les uns à la suite des autres, dans l'ordre des rangs. C'est
    l'opération inverse de l'exemple `Scatter`.

!!! warning "Une erreur fréquente"

    Utiliser le résultat de `gather` sur un autre rang que `root` : il y
    vaut `None`. Le traitement des données collectées se fait dans un
    `if rank == root:`, ou bien on utilise
    [`allgather`](#collecte-pour-tous-allgather).

!!! example "Exercice 7 — Utilisation de `gather`"

    Mettre en pratique la communication collective `gather`.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/gather_com)

    Dans votre éditeur : `exercices/mpi/python/gather_com/README.md`.

## Collecte pour tous : `allgather`

Comme `gather`, mais **tous les rangs** reçoivent la liste complète. Il
n'y a pas de `root`.

--8<-- "mpi/allgather.svg"

=== "Objets Python"

    ```python
    # Sur chaque rang : liste des valeurs de tous les rangs
    values = comm.allgather(local_value)
    ```

=== "Tableaux NumPy"

    ```python
    comm.Allgather(send_buffer, recv_buffer)
    ```

    Le tableau `recv_buffer` doit exister **sur tous les rangs**, avec une
    taille égale au nombre de rangs multiplié par la taille de
    `send_buffer`.

`allgather` revient à faire un `gather` suivi d'un `bcast` du résultat,
mais en un seul appel, que MPI peut optimiser.

### Exemples de collecte pour tous

=== "Une valeur par rang"

    ```python
    noms = comm.allgather(f"proc-{rank}")
    # rang 0 envoie "proc-0", rang 1 envoie "proc-1", ...
    ```

    Avec 4 processus, sur **chaque** rang, `noms` vaut
    `['proc-0', 'proc-1', 'proc-2', 'proc-3']`.

=== "Connaître la taille des données des autres"

    Un usage courant : chaque rang annonce combien de valeurs il possède,
    et tous en déduisent `count` et `displacement`, utiles par exemple
    pour [`Gatherv`](collectives-avancees.md#collecte-de-tailles-variables-gatherv).

    ```python
    size = comm.Get_size()

    n_local = rank + 1
    # rang 0 : 1 valeur, rang 1 : 2 valeurs, rang 2 : 3, rang 3 : 4

    count = comm.allgather(n_local)
    # count = [1, 2, 3, 4] sur tous les rangs

    displacement = [sum(count[:i]) for i in range(size)]
    # displacement = [0, 1, 3, 6] sur tous les rangs
    ```

=== "Reconstruire un tableau NumPy partout"

    ```python
    import numpy as np

    size = comm.Get_size()

    local = np.array([float(rank), float(10 * rank)])
    # rang 0 : [0. 0.], rang 1 : [1. 10.], rang 2 : [2. 20.], ...

    full = np.empty(2 * size, dtype=np.float64)   # sur tous les rangs
    comm.Allgather(local, full)
    ```

    Avec 4 processus, sur **chaque** rang, `full` vaut
    `[0. 0. 1. 10. 2. 20. 3. 30.]`.

## Bilan

!!! success "À ce stade, vous savez"

    - diffuser une donnée à tous les processus avec `bcast` ;
    - distribuer des données entre les processus avec `scatter` ;
    - rassembler des données avec `gather` et `allgather`.

La suite : les [réductions](reductions.md).
