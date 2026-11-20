# 5.2 Communications collectives : réductions

Une **réduction** rassemble des données réparties sur plusieurs processus
en les combinant au passage par une opération (somme, maximum…). C'est le
troisième type de communication collective présenté dans la
[partie précédente](collectives.md#les-differents-types).

## Réduction : `reduce`

Envoi de données réparties sur plusieurs processus vers **un seul
processus**, avec une **opération de réduction** (somme, produit,
maximum…) réalisée au passage.

--8<-- "mpi/reduce.svg"

`reduce` est appelée par tous les processus en même temps :

```python
reduced = comm.reduce(local_value, op=MPI.SUM, root=0)
```

| Paramètre | Rôle |
|---|---|
| `local_value` | Valeur envoyée par chaque processus |
| `reduced` | Résultat de la réduction, sur `root` seulement (`None` ailleurs) |
| `op` | Opération de réduction |
| `root` | Processus qui reçoit le résultat |

:material-book-open-variant: [Documentation de `Comm.reduce`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.reduce)

### Opérations de réduction

MPI fournit des opérations **prédéfinies**. Dans les exemples du tableau,
4 rangs envoient respectivement `3.0`, `7.5`, `1.2` et `5.0`.

| Opération | Effet | Exemple |
|---|---|---|
| **Arithmétiques** | | |
| `MPI.SUM` | Somme | `16.7` |
| `MPI.PROD` | Produit | `135.0` |
| `MPI.MAX` | Maximum | `7.5` |
| `MPI.MIN` | Minimum | `1.2` |
| **Avec position** | | |
| `MPI.MAXLOC` | Maximum **et** rang qui le possède | `(7.5, 1)` |
| `MPI.MINLOC` | Minimum **et** rang qui le possède | `(1.2, 2)` |
| **Logiques** (sur des booléens) | | |
| `MPI.LAND` | ET logique : vrai si **tous** sont vrais | |
| `MPI.LOR` | OU logique : vrai si **au moins un** est vrai | |
| `MPI.LXOR` | OU exclusif : vrai si un nombre **impair** est vrai | |
| **Bit à bit** (sur des entiers) | | |
| `MPI.BAND` | ET bit à bit | |
| `MPI.BOR` | OU bit à bit | |
| `MPI.BXOR` | OU exclusif bit à bit | |

### Exemples de réduction

=== "Somme sur le rang 0"

    ```python
    local_value = 125.87 * rank
    total = comm.reduce(local_value, op=MPI.SUM, root=0)
    # Avec 4 rangs, sur le rang 0 : 125.87 × (0 + 1 + 2 + 3) = 755.22
    ```

=== "Produit sur le rang 3"

    ```python
    local_value = 5 * rank
    prod = comm.reduce(local_value, op=MPI.PROD, root=3)
    # Sur le rang 3 : 0, car le rang 0 envoie 5 × 0 = 0
    ```

=== "Maximum d'un tableau"

    ```python
    import numpy as np

    # Tableau stocké sur chaque processus
    local_array = np.random.rand(5)

    # Pour chaque indice, maximum sur l'ensemble des processus
    max_array = comm.reduce(local_array, op=MPI.MAX, root=0)

    # Puis maximum sur tous les indices
    if rank == 0:
        global_max = np.max(max_array)
    ```

=== "Position du maximum : `MAXLOC`"

    `MAXLOC` et `MINLOC` réduisent des couples `(valeur, indice)`. On
    passe en général le rang comme indice, pour savoir **qui** possède le
    maximum :

    ```python
    local_value = [3.0, 7.5, 1.2, 5.0][rank]

    result = comm.reduce((local_value, rank), op=MPI.MAXLOC, root=0)
    # Sur le rang 0 : (7.5, 1), le maximum vaut 7.5 et c'est le rang 1
    ```

=== "Tests logiques : `LAND` et `LOR`"

    Les opérations logiques servent par exemple à vérifier une condition
    sur tous les rangs, comme la convergence d'un calcul itératif :

    ```python
    converged = residu_local < 1e-6      # booléen propre à chaque rang

    # Sur le rang 0 : vrai seulement si tous les rangs ont convergé
    all_converged = comm.reduce(converged, op=MPI.LAND, root=0)

    # Sur le rang 0 : vrai si au moins un rang a rencontré une erreur
    any_error = comm.reduce(erreur_locale, op=MPI.LOR, root=0)
    ```

### Créer son propre opérateur de réduction

Quand aucune opération prédéfinie ne convient, on peut définir la sienne.
Avec mpi4py, la méthode dépend du type de données réduites.

=== "Objets Python : `reduce`"

    Avec `reduce` et `allreduce` en minuscules, il suffit de passer une
    **fonction Python à deux arguments**, qui combine deux valeurs et
    renvoie le résultat :

    ```python
    def concat(a, b):
        return a + b

    words = comm.reduce([f"r{rank}"], op=concat, root=0)
    # Sur le rang 0 : ['r0', 'r1', 'r2', 'r3']
    ```

    MPI appelle la fonction autant de fois que nécessaire pour combiner
    les valeurs de tous les rangs, deux par deux.

=== "Tableaux NumPy : `Reduce` et `MPI.Op.Create`"

    Avec les fonctions à majuscule (`Reduce`, `Allreduce`), on crée un
    **objet opérateur** avec `MPI.Op.Create`. La fonction reçoit alors
    trois arguments :

    - `inbuf` : les données d'un rang, en lecture seule ;
    - `inoutbuf` : les données d'un autre rang, où il faut **écrire le
      résultat** (on modifie ce tampon au lieu de renvoyer une valeur) ;
    - `datatype` : le type MPI des données.

    Les deux tampons sont des zones mémoire brutes : on les convertit en
    tableaux NumPy avec `np.frombuffer`.

    ```python
    import numpy as np

    # Somme des valeurs absolues, élément par élément
    def abs_sum(inbuf, inoutbuf, datatype):
        a = np.frombuffer(inbuf, dtype=np.float64)
        b = np.frombuffer(inoutbuf, dtype=np.float64)
        b[:] = np.abs(a) + np.abs(b)       # résultat écrit dans inoutbuf

    # Création de l'opérateur (commutatif ici)
    ABS_SUM = MPI.Op.Create(abs_sum, commute=True)

    local = np.array([(-1.0) ** rank * (rank + 1), -2.0 * rank])
    # rang 0 : [ 1.  0.], rang 1 : [-2. -2.], rang 2 : [ 3. -4.], rang 3 : [-4. -6.]

    result = np.empty(2) if rank == 0 else None
    comm.Reduce(local, result, op=ABS_SUM, root=0)
    # Sur le rang 0 : [10. 12.]

    # Libération de l'opérateur quand on n'en a plus besoin
    ABS_SUM.Free()
    ```

!!! warning "Les règles à respecter"

    - **Associativité.** L'opération doit être associative : MPI regroupe
      les calculs comme il veut, par exemple `(a + b) + (c + d)` au lieu de
      `((a + b) + c) + d`.
    - **Commutativité.** Avec `MPI.Op.Create`, le paramètre `commute`
      indique si l'ordre des opérandes est indifférent. Avec
      `commute=False`, MPI combine les valeurs dans l'ordre des rangs.
    - **Ne pas mélanger les deux méthodes.** Un opérateur créé avec
      `MPI.Op.Create` ne fonctionne pas avec `reduce` en minuscules, et une
      fonction à deux arguments ne fonctionne pas avec `Reduce` : on
      obtient une `TypeError`.

## Réduction pour tous : `allreduce`

Comme `reduce`, mais le résultat est envoyé à **tous** les processus.

--8<-- "mpi/allreduce.svg"

```python
total = comm.allreduce(local_value, op=MPI.SUM)
```

Il n'y a pas de `root` : chaque processus reçoit le résultat de la
réduction.

:material-book-open-variant: [Documentation de `Comm.allreduce`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.allreduce)

!!! example "Exercice 8 — Utilisation de `reduce`"

    Calculer une intégrale en parallèle à l'aide d'une réduction.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/reduce_com)

    Dans votre éditeur : `exercices/mpi/python/reduce_com/README.md`.

## Bilan

!!! success "À ce stade, vous savez"

    - effectuer une réduction avec `reduce` et `allreduce` ;
    - choisir l'opération de réduction adaptée ;
    - créer votre propre opérateur de réduction.

La suite : les [variantes et les collectives non bloquantes](collectives-avancees.md).
