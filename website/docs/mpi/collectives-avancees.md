# 5.3 Communications collectives : tailles variables et non bloquantes

## Données de taille variable : les variantes `v`

Certaines communications collectives (hors réductions) imposent que
chaque rang échange la **même quantité** de données. Cette limitation est
levée par des variantes, dont le nom se termine par un **v** :
`Gatherv`, `Scatterv`, `Allgatherv`, `Alltoallv`.

### Collecte de tailles variables : `Gatherv`

`Gatherv` envoie des données de **tailles différentes**, réparties sur
plusieurs processus, vers un processus unique. Contrairement à `Gather`,
qui range les contributions dans l'ordre des rangs, `Gatherv` permet
aussi de **choisir où placer** chaque contribution.

```python
comm.Gatherv(send_buffer, [recv_buffer, count, displacement, MPI.DOUBLE], root=0)
```

| Paramètre | Rôle |
|---|---|
| `send_buffer` | Tableau envoyé par chaque processus |
| `recv_buffer` | Tableau réunissant toutes les contributions (sur `root`) |
| `count` | Liste du nombre de valeurs envoyées par chaque rang |
| `displacement` | Position, dans `recv_buffer`, de la première valeur venant de chaque rang |
| `MPI.DOUBLE` | Type des données reçues |
| `root` | Processus qui reçoit les données |

Les listes `count` et `displacement` ont pour taille le nombre de rangs
du communicateur. Avec les mêmes contributions, trois choix de
`displacement` donnent trois rangements différents :

--8<-- "mpi/gatherv.svg"

## Communications collectives non bloquantes

Toutes les communications collectives présentées ont aussi un équivalent
**non bloquant**, dont le nom commence par un **I** : `Ibcast`,
`Iscatter`, `Igather`, `Igatherv`, `Ireduce`, `Iallreduce`, `Ialltoall`…
Elles fonctionnent comme les communications point à point non bloquantes
(`isend`, `irecv`) :

- l'appel **rend la main tout de suite** et renvoie une **requête** ;
- la communication se déroule **en arrière-plan**, pendant que le
  processus continue à calculer : c'est le **recouvrement** des
  communications par le calcul ;
- il faut appeler `Wait` sur la requête **avant d'utiliser le résultat**.

Avec mpi4py, ces fonctions existent dans la version à majuscule, qui
travaille sur des tableaux NumPy.

### Exemple : une réduction pendant un calcul

Chaque rang calcule une somme partielle. La somme totale est lancée avec
`Iallreduce`, et chaque rang effectue un autre calcul, qui n'en dépend
pas, pendant que la réduction a lieu :

```python
import numpy as np
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

# Chaque rang calcule une somme partielle
local_sum = np.array([float(rank + 1)])
total = np.empty(1)

# Lancement de la réduction : la fonction rend la main tout de suite
req = comm.Iallreduce(local_sum, total, op=MPI.SUM)

# Calcul qui n'a pas besoin de total : il recouvre la communication
other = np.sum(np.arange(1_000_000) * 0.5)

# On attend la fin de la réduction avant d'utiliser total
req.Wait()
print(f"Rang {rank} : total = {total[0]}")
```

Avec 4 processus, chaque rang affiche `total = 10.0` (1 + 2 + 3 + 4).

### Exemple : plusieurs collectives en même temps

On peut lancer plusieurs collectives non bloquantes à la suite, puis les
attendre toutes ensemble avec `Waitall` :

```python
data = np.arange(4, dtype=np.float64) if rank == 0 else np.empty(4)
maximum = np.empty(1)

req_bcast = comm.Ibcast(data, root=0)
req_max = comm.Iallreduce(np.array([float(rank)]), maximum, op=MPI.MAX)

MPI.Request.Waitall([req_bcast, req_max])
```

Après le `Waitall`, tous les rangs ont reçu le tableau `data` du rang 0
et connaissent le plus grand rang (`maximum`).

!!! warning "Les règles à respecter"

    - **Tous les processus** du communicateur doivent appeler la même
      collective, **dans le même ordre**, comme pour les versions
      bloquantes.
    - Avant le `Wait`, on ne doit **ni lire le tampon de réception**
      (`total`), **ni modifier le tampon d'envoi** (`local_sum`).
    - Le calcul lancé pendant la communication ne doit pas dépendre de
      son résultat : sinon, il n'y a rien à recouvrir.

D'autres variantes sont à découvrir dans le
[cours de l'IDRIS](http://www.idris.fr/formations/mpi/) et la
documentation de MPI.

## Bilan

!!! success "À ce stade, vous savez"

    - collecter des données de tailles différentes avec `Gatherv` ;
    - lancer des communications collectives non bloquantes et les
      attendre avec `Wait` et `Waitall`.
