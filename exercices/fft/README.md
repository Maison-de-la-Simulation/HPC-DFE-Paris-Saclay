# Exercice : équation de Poisson par FFT distribuée

Dans cet exercice, vous allez résoudre l'équation de Poisson en 2D par deux
méthodes, puis comparer leur temps d'exécution :

1. la méthode simple, qui **rassemble tout le domaine sur le rang 0** ;
2. la **FFT distribuée**, avec la bibliothèque `mpi4py-fft`.

Cours associé : [FFT distribuée avec mpi4py-fft](https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/fft/index.html)

## Le problème

On résout, sur le domaine périodique \[0, Lx\] × \[0, Ly\] :

```text
∇²ψ = -ω
```

En Fourier, la solution s'écrit simplement :

```text
ψ_hat = ω_hat / k²,     avec k² = kx² + ky²
```

Le second membre ω est une somme de sinus et de cosinus, choisie pour que la
**solution exacte** ψ soit connue : le code peut ainsi mesurer l'erreur de
chaque méthode.

Le domaine est découpé en **bandes de lignes en x**, une par rang MPI : chaque
rang possède `omega_local`, un tableau de `Nx // size` × `Ny` valeurs. `Nx` doit
être divisible par le nombre de rangs.

## Fichiers fournis

| Fichier | Rôle |
|---|---|
| `main.py` | Code de départ, déjà en MPI : découpage du domaine, second membre, solution exacte. Les deux méthodes sont à compléter |
| `correction/main.py` | La correction, avec le bonus |
| `requirements.txt` | Les bibliothèques Python nécessaires |

Pour chaque méthode, le code mesure le temps de résolution avec `MPI.Wtime()`
et affiche l'erreur maximale par rapport à la solution exacte, dès que la
variable `psi_local` est remplie.

## Prérequis

L'exercice nécessite `numpy`, `mpi4py` et `mpi4py-fft`, listés dans
[requirements.txt](./requirements.txt) :

```bash
pip install -r requirements.txt
```

`mpi4py-fft` s'appuie sur la bibliothèque FFTW, qu'il faut installer avant :
voir l'étape 5 du
[guide d'installation](https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/installation.html#5-installer-mpi4py-fft-facultatif).

## Consignes

### 0. Prendre en main le code de départ

1. Lisez `main.py` : repérez le découpage en bandes, le second membre
   `omega_local`, la solution exacte `psi_exact_local`, les nombres d'onde
   globaux `kx` et `ky`, et les zones `À compléter` des deux méthodes.

2. Exécutez le code :

   ```bash
   mpirun -np 4 python main.py
   ```

   Il n'affiche encore aucun résultat : les deux méthodes sont à écrire.

### 1. Méthode 1 : tout rassembler sur le rang 0

Dans la section `Méthode 1`, entre les deux mesures du temps :

1. Rassemblez les bandes `omega_local` de tous les rangs sur le rang 0, avec
   `comm.gather`.

2. Sur le rang 0 seulement :
   - reconstruisez le tableau global `(Nx, Ny)` avec `np.vstack` ;
   - construisez `K2 = KX² + KY²` à partir de `kx` et `ky`, en remplaçant le
     mode (0, 0) par 1 pour éviter la division par 0 ;
   - calculez `psi_hat = np.fft.fft2(omega_global) / K2`, puis
     `psi_global = np.real(np.fft.ifft2(psi_hat))` ;
   - découpez `psi_global` en une bande par rang.

3. Redistribuez les bandes avec `comm.scatter` dans `psi_local`.

4. Exécutez : l'erreur doit être de l'ordre de `1e-17`, car la FFT est exacte
   pour ce second membre.

5. Quelle quantité de mémoire le rang 0 doit-il avoir ? Que font les autres
   rangs pendant que le rang 0 calcule ?

### 2. Méthode 2 : préparer la FFT distribuée

Dans la partie `Préparation` de la section `Méthode 2`, hors de la mesure du
temps (créer un plan de FFT est coûteux, on ne le fait qu'une fois) :

1. Créez le plan de FFT 2D avec
   `PFFT(comm, (Nx, Ny), axes=(0, 1), dtype=np.float64)`.

2. Créez les tableaux distribués avec `newDistArray` : `omega_dist` et
   `psi_dist` dans l'espace physique, `omega_hat` et `psi_hat` dans l'espace
   spectral.

3. Vérifiez que le découpage de `mpi4py-fft` correspond à celui du code :
   `omega_dist.shape` doit être égal à `omega_local.shape`. Sinon, arrêtez le
   programme avec un message clair.

4. Calculez les nombres d'onde de la partie spectrale locale :
   - les données étant réelles, la direction y n'a que `Ny // 2 + 1` modes
     spectraux : calculez les nombres d'onde en y avec `np.fft.rfftfreq` ;
   - avec `omega_hat.local_slice()`, récupérez les indices de la partie
     spectrale locale et construisez `K2_loc = KX² + KY²` ;
   - remplacez le mode (0, 0) par 1. Attention, ce mode n'appartient qu'à
     **un seul** rang.

### 3. Méthode 2 : résoudre avec la FFT distribuée

Dans la partie `Résolution` de la section `Méthode 2` :

1. Copiez `omega_local` dans le tableau distribué `omega_dist`.
2. Calculez la FFT directe avec `fft.forward`.
3. Divisez `omega_hat` par `K2_loc` dans `psi_hat`.
4. Calculez la FFT inverse avec `fft.backward`.
5. Copiez le résultat dans `psi_local` (`np.array(psi_dist, copy=True)`).

Exécutez : l'erreur doit être du même ordre que celle de la méthode 1.
Vérifiez-le pour 1, 2 et 4 processus.

### 4. Comparer les performances

1. Comparez les temps des deux méthodes sur une grande grille, par exemple :

   ```bash
   mpirun -np 4 python main.py --Nx 1024 --Ny 1024
   ```

2. Sur Ruche, faites varier le nombre de processus (1, 2, 4, 8, 16, 32) et
   tracez le temps des deux méthodes.

3. Commentez :
   - Pourquoi le temps de la méthode 1 ne diminue-t-il pas avec le nombre de
     processus ?
   - Quelle étape de la FFT distribuée nécessite des communications ?
   - Quelle méthode utiliseriez-vous pour une simulation sur des milliers de
     processus ?
