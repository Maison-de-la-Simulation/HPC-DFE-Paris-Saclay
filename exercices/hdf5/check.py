# ______________________________________________________________________________
#
# Vérification des fichiers produits par l'exercice HDF5
#
# Usage :
#   python check.py methode2.h5           # un seul fichier (méthodes 2 et 3)
#   python check.py methode1_rang_*.h5    # un fichier par rang (méthode 1)
#   python check.py methode2.h5 --save champ.png
#                                         # sans affichage : enregistre l'image
#
# Le script reconstruit le tableau global, vérifie que chaque bande de lignes
# contient le numéro du rang attendu, puis affiche le tableau avec matplotlib.
# L'option --save remplace l'affichage par l'enregistrement d'une image PNG,
# par exemple sans écran (intégration continue, connexion sans affichage).
# ______________________________________________________________________________

import argparse
import sys

import h5py
import matplotlib.pyplot as plt
import numpy as np


def block_rows(Nx, number_of_ranks, rank):
    """Même découpage que dans main.py."""
    n_local = Nx // number_of_ranks + (1 if rank < Nx % number_of_ranks else 0)
    start = rank * (Nx // number_of_ranks) + min(rank, Nx % number_of_ranks)
    return start, n_local


parser = argparse.ArgumentParser(description="Vérifie les fichiers de l'exercice HDF5")
parser.add_argument("files", nargs="+",
                    help="un fichier unique, ou un fichier par rang (methode1_rang_*.h5)")
parser.add_argument("--save", metavar="IMAGE",
                    help="n'affiche pas la figure et l'enregistre dans ce fichier PNG")
args = parser.parse_args()

if args.save:
    # Pas d'affichage : backend sans fenêtre, à choisir avant de créer la figure
    plt.switch_backend("Agg")

files = sorted(args.files)

# ______________________________________________________________________________
# Lecture

if len(files) == 1:
    # Un seul fichier contenant le tableau global
    with h5py.File(files[0], "r") as f:
        champ = f["champ"][...]
        number_of_ranks = int(f["champ"].attrs["number_of_ranks"])
else:
    # Un fichier par rang : chaque fichier contient sa bande et sa position
    blocks = []
    for name in files:
        with h5py.File(name, "r") as f:
            dset = f["champ"]
            blocks.append((int(dset.attrs["start"]), dset[...],
                           tuple(dset.attrs["global_shape"]),
                           int(dset.attrs["number_of_ranks"])))
    global_shape = blocks[0][2]
    number_of_ranks = blocks[0][3]
    if len(blocks) != number_of_ranks:
        sys.exit(f"Erreur : {len(blocks)} fichiers pour {number_of_ranks} rangs")
    champ = np.empty(global_shape, dtype=np.float64)
    for start, data, _, _ in blocks:
        champ[start:start + data.shape[0], :] = data

# ______________________________________________________________________________
# Vérification

Nx, Ny = champ.shape
ok = True
for rank in range(number_of_ranks):
    start, n_local = block_rows(Nx, number_of_ranks, rank)
    if not np.all(champ[start:start + n_local, :] == rank):
        print(f"Erreur : les lignes {start} à {start + n_local - 1} ne valent pas {rank}")
        ok = False

print(f"Tableau {Nx} x {Ny}, {number_of_ranks} rangs : "
      + ("correct" if ok else "INCORRECT"))

# ______________________________________________________________________________
# Affichage

fig, ax = plt.subplots(figsize=(6, 4))
im = ax.imshow(champ, cmap="viridis", aspect="auto")
fig.colorbar(im, ax=ax, label="rang")
ax.set_xlabel("colonne")
ax.set_ylabel("ligne")
ax.set_title("Bande de lignes écrite par chaque rang")
if args.save:
    fig.savefig(args.save, dpi=100, bbox_inches="tight")
    print(f"Image enregistrée dans {args.save}")
else:
    plt.show()

sys.exit(0 if ok else 1)
