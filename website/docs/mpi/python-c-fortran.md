# 6. Python face au C et au Fortran

MPI a été conçu pour le C et le Fortran, les langages historiques du
calcul scientifique. mpi4py en offre l'accès depuis Python, avec quelques
différences qu'il faut connaître pour lire du code MPI écrit dans d'autres
langages et pour écrire du code Python efficace.

## Le même échange dans les trois langages

Voici l'envoi d'un tableau de 1000 réels du rang 0 vers le rang 1 :

=== "Python (mpi4py)"

    ```python
    import numpy as np
    from mpi4py import MPI

    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()

    data = np.empty(1000, dtype=np.float64)

    if rank == 0:
        comm.Send(data, dest=1, tag=0)
    elif rank == 1:
        comm.Recv(data, source=0, tag=0)
    ```

=== "C"

    ```c
    #include <mpi.h>

    int main(int argc, char **argv) {
        int rank;
        double data[1000];

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0)
            MPI_Send(data, 1000, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        else if (rank == 1)
            MPI_Recv(data, 1000, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

        MPI_Finalize();
        return 0;
    }
    ```

=== "Fortran"

    ```fortran
    program echange
        use mpi_f08
        implicit none
        integer :: rank
        real(8) :: data(1000)

        call MPI_Init()
        call MPI_Comm_rank(MPI_COMM_WORLD, rank)

        if (rank == 0) then
            call MPI_Send(data, 1000, MPI_DOUBLE_PRECISION, 1, 0, MPI_COMM_WORLD)
        else if (rank == 1) then
            call MPI_Recv(data, 1000, MPI_DOUBLE_PRECISION, 0, 0, MPI_COMM_WORLD, &
                          MPI_STATUS_IGNORE)
        end if

        call MPI_Finalize()
    end program echange
    ```

Les principales différences :

| | C et Fortran | Python (mpi4py) |
|---|---|---|
| Nom des fonctions | `MPI_Send`, `MPI_Recv`… | méthodes du communicateur : `comm.Send`, `comm.Recv`… |
| Communicateur | passé en argument (`MPI_COMM_WORLD`) | objet sur lequel on appelle la méthode (`comm`) |
| Nombre d'éléments et type | donnés explicitement (`1000`, `MPI_DOUBLE`) | déduits du tableau NumPy |
| Initialisation | `MPI_Init` obligatoire | implicite à l'import, sauf si on la rend explicite comme dans ce cours |
| Compilation | le programme est compilé et lié à MPI | aucune : le script est interprété |
| Objets Python quelconques | impossible : uniquement des tampons mémoire | possible avec les fonctions en minuscules |

La dernière ligne est la vraie particularité de mpi4py : en plus des
fonctions qui correspondent au C et au Fortran, il propose des versions
en **minuscules** qui n'ont pas d'équivalent dans ces langages.

## Deux familles de fonctions : minuscules et majuscules

Dans mpi4py, la plupart des fonctions de communication existent en deux
versions : en **minuscules** (`send`, `recv`, `bcast`, `reduce`…) et avec
une **majuscule** (`Send`, `Recv`, `Bcast`, `Reduce`…). La différence
porte sur **ce que MPI envoie**.

--8<-- "mpi/minuscule-majuscule.svg"

### En minuscules : des objets Python

mpi4py **sérialise** l'objet avec le module `pickle` : il le convertit en
une suite d'octets, envoie ces octets, et le destinataire reconstruit un
nouvel objet à partir d'eux.

- On peut envoyer **n'importe quel objet** Python : un nombre, une liste,
  un dictionnaire, un tableau NumPy…
- Le résultat est **renvoyé** par la fonction ; il n'y a rien à préparer
  à l'avance, ni la taille ni le type.
- La conversion, la reconstruction et les copies en mémoire ont un
  **coût**, qui devient important pour de gros volumes de données.

```python
if rank == 0:
    comm.send({"dt": 1e-3, "Nx": 256}, dest=1)
elif rank == 1:
    params = comm.recv(source=0)       # params est renvoyé par recv
```

### Avec une majuscule : des tableaux en mémoire

C'est l'interface d'origine de MPI, celle du C et du Fortran. MPI lit
**directement la mémoire** du tableau envoyé et écrit **directement** dans
celle du tableau de réception.

- Il n'y a **aucune conversion** : la vitesse est proche de celle du C.
- Il faut **préparer soi-même le tableau de réception**, avec la bonne
  taille et le bon type. La fonction ne renvoie rien : elle **remplit** ce
  tableau.
- Cela ne fonctionne qu'avec des données **contiguës en mémoire**, en
  pratique des tableaux NumPy.
- Certaines fonctions n'existent que sous cette forme, comme les variantes
  `Gatherv` et `Scatterv`, présentées dans la partie sur les
  [communications collectives avancées](collectives-avancees.md).

```python
import numpy as np

if rank == 0:
    data = np.arange(1000, dtype=np.float64)
    comm.Send(data, dest=1)
elif rank == 1:
    data = np.empty(1000, dtype=np.float64)   # préparé à l'avance
    comm.Recv(data, source=0)                 # data est rempli par Recv
```

### Quel écart de performance ?

Temps moyen pour envoyer un tableau de réels entre 2 processus, mesuré sur
un ordinateur portable :

| Taille du tableau | `send` (minuscules) | `Send` (majuscule) |
|---|---|---|
| 10 valeurs | 0,08 ms | 0,04 ms |
| 10 millions de valeurs (80 Mo) | 28 ms | 4,9 ms |

Sur un petit message, la différence ne compte pas. Sur un gros tableau, la
version à majuscule est ici environ **6 fois plus rapide**.

!!! tip "Laquelle choisir ?"

    - **Minuscules** pour les petites données et les objets Python : un
      paramètre, un dictionnaire de configuration, un résultat scalaire.
    - **Majuscule** dès que l'on échange de **gros tableaux NumPy**, ce
      qui est le cas typique des simulations et des TPs de ce cours.

:material-book-open-variant: [Tutoriel mpi4py](https://mpi4py.readthedocs.io/en/stable/tutorial.html)
