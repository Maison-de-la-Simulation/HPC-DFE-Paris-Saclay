# 3. Communications point à point bloquantes

## Notion de communication point à point

L'échange de messages entre deux processus est la base de MPI. Il se
décompose toujours en **deux étapes** :

- **Envoi** : un processus envoie un message à un destinataire, désigné
  par son rang.
- **Réception** : le destinataire doit **explicitement** recevoir le
  message, en indiquant le rang de l'expéditeur.

--8<-- "mpi/p2p.svg"

## Envoyer : `send`

`send` est appelée par le processus **expéditeur** :

```python
comm.send(message, dest=destination, tag=tag)
```

| Paramètre | Rôle |
|---|---|
| `message` | Données à envoyer à un autre processus |
| `dest` (`int`) | Rang du processus destinataire |
| `tag` (`int`) | Numéro attribué à la communication, pour distinguer plusieurs messages vers le même processus |

:material-book-open-variant: [Documentation de `Comm.send`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.send)

!!! note "Le tag"

    Le tag permet de différencier des communications, mais cet aspect ne
    sera pas exploité dans ce cours. On peut lui donner une valeur par
    défaut (0 par exemple). À la réception, `MPI.ANY_TAG` permet
    d'accepter un message quel que soit son tag.

Avec mpi4py, `send` accepte n'importe quel objet Python :

=== "Un réel"

    Le rang 2 envoie un réel au rang 3 :

    ```python
    message = 1234.56
    if rank == 2:
        comm.send(message, dest=3, tag=0)
    ```

=== "Une liste"

    Le rang 1 envoie une liste d'entiers au rang 5 :

    ```python
    message = [12, 45, 67, 34, 54]
    if rank == 1:
        comm.send(message, dest=5, tag=0)
    ```

=== "Un dictionnaire"

    Le rang 1 envoie un dictionnaire au rang 6 :

    ```python
    message = {"a": 1, "b": 2}
    if rank == 1:
        comm.send(message, dest=6, tag=0)
    ```

## Recevoir : `recv`

`recv` est appelée par le processus **destinataire** et renvoie le
message reçu :

```python
message = comm.recv(source=source, tag=tag)
```

| Paramètre | Rôle |
|---|---|
| `source` (`int`) | Rang du processus expéditeur |
| `tag` (`int`) | Numéro de la communication, identique à celui de l'envoi |

:material-book-open-variant: [Documentation de `Comm.recv`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.recv)

### Exemple complet

Le rang 2 envoie un réel au rang 3, qui le reçoit :

```python
tag = 0
source = 2
destination = 3

if rank == source:
    message = 1234.56
    comm.send(message, dest=destination, tag=tag)

if rank == destination:
    message = comm.recv(source=source, tag=tag)
```

!!! tip "Minuscules ou majuscules ?"

    `send` et `recv` envoient des objets Python ; `Send` et `Recv`
    envoient directement des tableaux NumPy, beaucoup plus vite pour de
    gros volumes. Voir
    [Deux familles de fonctions](python-c-fortran.md#deux-familles-de-fonctions-minuscules-et-majuscules)
    dans la partie 6.

## Anatomie d'une communication

Une communication se compose de plusieurs sous-étapes, qui prennent
chacune du temps :

--8<-- "mpi/comm-steps.svg"

!!! example "Exercice 3 — Communications bloquantes"

    Tester les communications bloquantes `send` et `recv`.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/blocking_com)

    Dans votre éditeur : `exercices/mpi/python/blocking_com/README.md`.

## Échange mutuel : `sendrecv`

Il est parfois nécessaire que deux processus **échangent** des données :
chacun envoie et reçoit. La fonction `sendrecv` combine l'envoi et la
réception en un seul appel :

```python
recv_message = comm.sendrecv(send_message, dest=destination, sendtag=sendtag,
                             source=source, recvtag=recvtag)
```

- `send_message` : données envoyées à `dest` ;
- `recv_message` : données reçues de `source` ;
- les autres paramètres sont les mêmes que pour `send` et `recv`.

:material-book-open-variant: [Documentation de `Comm.sendrecv`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.sendrecv)

Exemple : le rang 2 envoie un message au rang 3 et en reçoit un du rang 1.

```python
tag = 0
if rank == 2:
    recv_message = comm.sendrecv(21.78, dest=3, sendtag=tag,
                                 source=1, recvtag=tag)
```

Les rangs 1 et 3 doivent bien sûr effectuer les appels correspondants
(un envoi vers 2 pour le rang 1, une réception depuis 2 pour le rang 3).

### Communications chaînées

`sendrecv` est aussi indispensable pour les **communications chaînées**,
où chaque processus reçoit des données d'un processus A et en envoie à un
processus B distinct, comme dans un anneau :

--8<-- "mpi/ring.svg"

Avec `send` et `recv` séparés, il faudrait gérer manuellement l'ordre des
appels : si tous les processus commencent par un `recv`, chacun attend un
message qui ne part jamais.

!!! warning "Interblocage (*deadlock*)"

    Lorsqu'une communication n'arrive pas à son terme, le programme attend
    et peut rester **figé** indéfiniment, sans message d'erreur.

## Les barrières

### Barrière explicite

Il est parfois nécessaire d'imposer une étape de **synchronisation** : une
barrière qu'aucun processus ne franchit tant que tous ne l'ont pas
atteinte.

```python
comm.Barrier()
```

--8<-- "mpi/barrier.svg"

:material-book-open-variant: [Documentation de `Comm.Barrier`](https://mpi4py.readthedocs.io/en/stable/reference/mpi4py.MPI.Comm.html#mpi4py.MPI.Comm.Barrier)

### Barrière implicite

Certaines fonctions d'échange induisent des **barrières implicites** entre
les processus concernés. C'est le cas de `send`, `recv` et `sendrecv`,
d'où le nom de communications **bloquantes** : un processus en avance
attend son partenaire, et cette attente est une perte de ressources.

--8<-- "mpi/barrier-implicit.svg"

## Mesurer le temps d'une section parallèle

Les barrières et les communications point à point complètent la mesure du
temps vue dans la partie
[Premier programme parallèle](premier-programme.md#mesurer-le-temps-dexecution).

### Synchroniser avant de mesurer

Les processus n'atteignent pas tous le début de la section au même
moment. Pour mesurer le **temps total** d'une section, du point de vue de
l'ensemble du programme, on place des **barrières** au début et à la fin :

```python
comm.Barrier()                       # tous partent du même point
start = MPI.Wtime()

# Section à mesurer
...

comm.Barrier()                       # on attend le plus lent
total = MPI.Wtime() - start
```

Le temps mesuré correspond alors à celui du processus **le plus lent**,
qui détermine la durée réelle de la section. Avec l'exemple de la partie
précédente, où les rangs travaillent 0,1, 0,2, 0,3 et 0,4 s, `total` vaut
environ `0.4 s` sur tous les rangs.

!!! warning "Les barrières ont un coût"

    Une barrière force les processus à s'attendre. On l'ajoute pour
    **mesurer**, mais on évite d'en laisser dans les parties du code dont
    on veut la meilleure performance.

### Rassembler les temps sur un rang

Pour analyser les temps, on les rassemble en général sur le rang 0, avec
les communications point à point vues dans cette partie :

```python
if rank == 0:
    times = [elapsed]
    for source in range(1, comm.Get_size()):
        times.append(comm.recv(source=source))
    print(f"Min : {min(times):.3f} s, max : {max(times):.3f} s")
else:
    comm.send(elapsed, dest=0)
```

Dans la partie sur les [réductions](reductions.md), nous
verrons comment obtenir directement le minimum, le maximum ou la moyenne
en une seule instruction, avec `reduce`.

!!! example "Exercice 4 — Communication en anneau"

    Tester `sendrecv` au travers d'un anneau de communication.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/mpi/python/sendrecv)

    Dans votre éditeur : `exercices/mpi/python/sendrecv/README.md`.

## Bilan

!!! success "À ce stade, vous savez"

    - faire communiquer différents processus entre eux ;
    - gérer des chaînes de communication ;
    - demander une synchronisation explicite ;
    - mesurer le temps total d'une section et rassembler les temps sur un
      rang.
