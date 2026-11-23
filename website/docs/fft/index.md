# V. FFT distribuée avec mpi4py-fft

De nombreux codes de simulation utilisent la **transformée de Fourier
rapide** (FFT), par exemple pour résoudre une équation de Poisson. Quand le
domaine est réparti entre plusieurs processus MPI, il faut une FFT
**distribuée**. Ce chapitre présente le minimum pour en faire une avec la
bibliothèque [mpi4py-fft](https://mpi4py-fft.readthedocs.io/), et
l'applique à l'équation de Poisson du TP Kelvin-Helmholtz.

## Pourquoi une FFT distribuée ?

Une FFT est une opération **globale** : chaque coefficient de Fourier
dépend de **toutes** les valeurs du tableau. Si chaque rang calcule une
FFT sur sa seule partie du domaine, le résultat est faux.

Deux solutions sont possibles :

| Solution | Principe | Limite |
|---|---|---|
| **Tout rassembler sur le rang 0** | `gather` du tableau, FFT globale avec NumPy sur le rang 0, puis `scatter` du résultat | Le rang 0 stocke tout le domaine et calcule seul : cela ne passe pas à l'échelle |
| **FFT distribuée** | Chaque rang calcule une partie de la FFT, et les rangs échangent des données entre les étapes | Plus complexe à programmer, d'où l'intérêt d'une bibliothèque |

## Le principe : FFT 1D et transposition

Une FFT 2D se décompose en FFT 1D le long de chaque direction : d'abord
le long de y, puis le long de x. Avec un domaine découpé en **bandes de
lignes en x**, comme dans le TP :

--8<-- "fft/transposition.svg"

1. Chaque rang possède des lignes complètes en y : il calcule les **FFT 1D
   le long de y** sans communiquer.
2. Une **transposition globale** (une communication `Alltoall`)
   redistribue les données : chaque rang reçoit des colonnes complètes en x.
3. Chaque rang calcule les **FFT 1D le long de x**, de nouveau sans
   communiquer.

En sortie, les données sont donc réparties **différemment** de l'entrée :
en bandes de lignes en x dans l'espace physique, en bandes de colonnes en
ky dans l'espace spectral. La transformée inverse fait le chemin inverse.

mpi4py-fft réalise toutes ces étapes, y compris la transposition.

## La bibliothèque mpi4py-fft

L'installation est décrite à l'[étape 5 du guide d'installation](../installation.md#5-installer-mpi4py-fft-facultatif).
Le minimum à connaître tient en quatre éléments :

`PFFT(comm, shape, axes, dtype)`
:   Le **plan** de FFT : il décrit le tableau global (`shape`), les
    directions transformées (`axes`) et le type des données. Il choisit
    aussi comment répartir le tableau entre les rangs de `comm`.

`newDistArray(fft, forward_output=False)`
:   Un tableau **distribué** dans l'espace physique : chaque rang n'en
    possède que sa partie. Avec `forward_output=True`, c'est un tableau
    de l'espace spectral.

`array.local_slice()`
:   Les indices **globaux** de la partie locale, sous forme d'un tuple de
    `slice` : c'est ce qui permet de savoir quelle partie du tableau
    global chaque rang possède.

`fft.forward(u, u_hat)` et `fft.backward(u_hat, u)`
:   La FFT directe et la FFT inverse, distribuées.

!!! note "Les conventions à connaître"

    - **Données réelles :** avec `dtype=np.float64`, la FFT de la dernière
      direction (ici y) ne garde que `Ny // 2 + 1` coefficients, comme
      `numpy.fft.rfft2`. Les nombres d'onde en y s'obtiennent donc avec
      `np.fft.rfftfreq`.
    - **Normalisation :** la FFT directe divise par `Nx * Ny`, et la FFT
      inverse ne divise pas. L'aller-retour redonne le tableau de départ.
    - **Au moins deux dimensions :** `PFFT` répartit le tableau selon une
      direction et garde l'autre locale. Il ne sait pas calculer la FFT
      d'un seul signal 1D réparti entre les processus.

## Exemple : FFT 2D distribuée

```python
import numpy as np
from mpi4py import MPI
from mpi4py_fft import PFFT, newDistArray

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

Nx, Ny = 16, 8

# Plan de FFT 2D, sur les deux directions, pour des données réelles
fft = PFFT(comm, (Nx, Ny), axes=(0, 1), dtype=np.float64)

# Tableaux distribués : espace physique et espace spectral
u = newDistArray(fft, forward_output=False)
u_hat = newDistArray(fft, forward_output=True)

# Chaque rang remplit sa partie, repérée par ses indices globaux
sx, sy = u.local_slice()
X, Y = np.meshgrid(np.arange(Nx)[sx], np.arange(Ny)[sy], indexing="ij")
u[:] = np.cos(2 * np.pi * 2 * X / Nx) * np.sin(2 * np.pi * Y / Ny)

# FFT directe, puis inverse
fft.forward(u, u_hat)
v = newDistArray(fft, forward_output=False)
fft.backward(u_hat, v)          # v est égal à u

print(f"Rang {rank} : physique {u.shape} {u.local_slice()}, "
      f"spectral {u_hat.shape} {u_hat.local_slice()}")
```

Avec 2 processus, on obtient :

| Rang | Partie physique | Indices | Partie spectrale | Indices |
|---|---|---|---|---|
| 0 | 8 × 8 | lignes x 0 à 7, toutes les colonnes | 16 × 3 | tous les kx, ky 0 à 2 |
| 1 | 8 × 8 | lignes x 8 à 15, toutes les colonnes | 16 × 2 | tous les kx, ky 3 et 4 |

On retrouve la transposition : en espace physique, le tableau est coupé
selon x ; en espace spectral, selon ky. Les 5 colonnes spectrales
(`Ny // 2 + 1 = 5`) sont réparties en 3 et 2.

## Application : l'équation de Poisson du TP

Le TP Kelvin-Helmholtz résout l'équation \(\nabla^2 \psi = -\omega\) en
Fourier : \(\hat{\psi} = \hat{\omega} / k^2\), avec
\(k^2 = k_x^2 + k_y^2\). En version distribuée, chaque rang ne divise que
**ses** coefficients, par **ses** nombres d'onde :

```python
Nx, Ny = 16, 8
Lx, Ly = 2.0, 1.0
dx, dy = Lx / Nx, Ly / Ny

fft = PFFT(comm, (Nx, Ny), axes=(0, 1), dtype=np.float64)
omega = newDistArray(fft, forward_output=False)
omega_hat = newDistArray(fft, forward_output=True)
psi_hat = newDistArray(fft, forward_output=True)
psi = newDistArray(fft, forward_output=False)

# Nombres d'onde globaux (rfftfreq en y : données réelles)
kx = 2 * np.pi * np.fft.fftfreq(Nx, d=dx)
ky = 2 * np.pi * np.fft.rfftfreq(Ny, d=dy)

# Nombres d'onde de la partie spectrale locale
skx, sky = omega_hat.local_slice()
KX, KY = np.meshgrid(kx[skx], ky[sky], indexing="ij")
K2 = KX**2 + KY**2

# Le mode (0, 0) n'appartient qu'au rang qui possède le début des deux
# directions : lui seul évite la division par 0
if skx.start == 0 and sky.start == 0:
    K2[0, 0] = 1.0

# Résolution
fft.forward(omega, omega_hat)
psi_hat[:] = omega_hat / K2
fft.backward(psi_hat, psi)
```

## Bilan

!!! success "À ce stade, vous savez"

    - expliquer pourquoi une FFT ne peut pas être calculée localement sur
      chaque partie du domaine ;
    - décrire le principe d'une FFT 2D distribuée : FFT 1D locales et
      transposition ;
    - créer un plan et des tableaux distribués avec mpi4py-fft, et repérer
      la partie locale avec `local_slice` ;
    - résoudre une équation de Poisson avec une FFT distribuée.
