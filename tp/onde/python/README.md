# Analyse des fichiers de sortie

L'éxecution du code genère des fichiers de sortie HDF5 dans un dossier `diags`
qui sera créé au début de chaque *run*.
Les fichiers de sortie portent le nom suivant `grid_<numéro d'itération>.dat`.
Ce dossier contient des scripts Python
dédiés à l'analyse des résultats.
Ils permettent d'ouvrir les fichiers de sortie pour visualiser les résultats :
- `plot.py` : ce fichier permet de lire et d'afficher les résultats
d'un fichier à un instant donné en 2D.

```bash
# Dans le dossier de simulation, cette commande affichera
# la grille à l'itération 100 si le fichier existe.
python ../python/plot.py diags/grid_00100.dat
```

![alt text](../extra/diffusion_plot2d.png "2D plot of the diffusion grid")

- `plot3d.py` : ce fichier permet de lire et d'afficher les résultats
d'un fichier à un instant donné en 3D.

![alt text](../extra/diffusion_plot3d.png "3D plot of the diffusion grid")

- `animate.py` : ce fichier permet de lire et d'afficher l'ensemble des fichiers du dossier `diags` en 2D sous la forme d'une animation.

```bash
# Dans le dossier de simulation, cette commande affichera
# la grille pour tous les fichiers dans diags
python ../python/animate.py diags
```

- `animate3d.py` : ce fichier permet de lire et d'afficher l'ensemble des fichiers du dossier `diags` en 3D sous la forme d'une animation.

```bash
# Dans le dossier de simulation, cette commande affichera
# la grille pour tous les fichiers dans diags en 3d
python ../python/animate3d.py diags
```

Pour vous épargner le chemin complet, vous pouvez tout simplement rajouter
un alias dans votre environnement:
```bash
alias plot="python <chemin vers le dossier python>/plot.py"
```

# Aide au tracé de figure

Des scripts sont également à votre disposition pour les courbes de scalabilité :
- `weak_scaling_omp.py`
- `weak_scaling_mpi.py`
- `strong_scaling_omp.py`
- `strong_scaling_mpi.py`