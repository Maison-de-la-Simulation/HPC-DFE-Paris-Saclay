# 3. Communications point à point non bloquantes

## Principe

Les communications **non bloquantes** évitent la synchronisation
implicite des processus : l'appel rend la main tout de suite, et l'échange
se déroule **en arrière-plan**.

- Le processus peut **calculer pendant que les données circulent** : on
  parle de **recouvrement** des communications par le calcul.
- Chaque communication reçoit un **identifiant unique**, la *requête*
  (*request*).
- Avant d'utiliser les données échangées, il faut vérifier que la
  communication est terminée : c'est le rôle de `wait`, qui impose alors
  une barrière.

--8<-- "mpi/nonblocking.svg"

## `isend` et `irecv`

`isend` est appelée par l'expéditeur, `irecv` par le destinataire. Les
paramètres sont les mêmes que pour les communications bloquantes ; les
deux fonctions renvoient une **requête** :

```python
req = comm.isend(message, dest=dest, tag=tag)
req = comm.irecv(source=source, tag=tag)
```

:material-book-open-variant: Documentation :
[`Comm.isend`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.isend) ·
[`Comm.irecv`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.irecv)

## Attendre la fin : `wait`

`wait`, appelée sur une requête, attend que la communication soit
effectivement terminée. Pour une réception, elle renvoie le message reçu :

```python
message = req.wait()
```

:material-book-open-variant: [Documentation de `Request.wait`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Request.html#mpi4py.MPI.Request.wait)

### Exemple

Le rang 2 envoie un réel au rang 3 ; chacun peut calculer avant
d'attendre la fin de l'échange :

```python
tag = 0

if rank == 2:
    req_send = comm.isend(1234.56, dest=3, tag=tag)
    # ... calcul qui n'a pas besoin du message ...
    req_send.wait()

if rank == 3:
    req_recv = comm.irecv(source=2, tag=tag)
    # ... calcul qui n'a pas besoin du message ...
    recv_message = req_recv.wait()
```

!!! warning "Ne pas toucher aux données avant le `wait`"

    Tant que `wait` n'a pas été appelé, le message reçu n'est pas
    disponible. Côté expéditeur, avec les fonctions à majuscule (`Isend`)
    qui envoient un tableau NumPy, il ne faut pas non plus modifier ce
    tableau avant la fin de l'envoi.

## Attendre plusieurs requêtes : `waitall`

`waitall` effectue l'action de `wait` sur une **liste de requêtes** :

```python
req_send = comm.isend(send_message, dest=dest_rank)
req_recv = comm.irecv(source=src_rank)

# waitall renvoie la liste des résultats, dans l'ordre des requêtes
_, recv_message = MPI.Request.waitall([req_send, req_recv])
```

## Mélanger les types de communication

- On peut tout à fait mélanger des appels bloquants et non bloquants :
  un message envoyé par `isend` peut être reçu par `recv`, et
  inversement.
- Une requête terminée devient `MPI.REQUEST_NULL`. On peut aussi
  initialiser certaines requêtes à `MPI.REQUEST_NULL` pour qu'elles soient
  ignorées par `wait` et `waitall`.

!!! example "Exercice 5 — Communications non bloquantes"

    Mettre en pratique `isend` et `irecv`.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/nonblocking_com)

    Dans votre éditeur : `exercices/mpi/python/nonblocking_com/README.md`.

!!! example "Exercice 6 — Communications de tableaux"

    Communiquer non plus un simple nombre mais un tableau NumPy.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/array_com)

    Dans votre éditeur : `exercices/mpi/python/array_com/README.md`.
