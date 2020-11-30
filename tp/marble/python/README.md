# Analyse des fichiers de sortie

L'éxecution du code genère plusieurs type de fichiers dans un dossier `diags`
qui sera créé au début de chaque *run* :
- fichiers hdf5 (`.h5`)
- fichiers bianire (`.bin`)
- fichiers vtk pour Paraview (`.vtk`)
Les fichiers de sortie portent le nom suivant `particles_<numéro d'itération>.bin`.
Ce dossier contient des scripts Python dédiés à l'analyse des résultats.
Ils permettent d'ouvrir les fichiers de sortie pour visualiser les résultats :
- `plot_binary.py` : ce fichier permet de lire et d'afficher les résultats d'un fichier à un instant donné en 3D.
- `plot_hdf5.py` : ce fichier permet de lire et d'afficher les résultats d'un fichier à un instant donné en 3D.

```bash
# Dans le dossier de simulation, cette commande affichera
# les particules à l'itération 100 si le fichier binaire existe.
python ../python/plot_binary.py diags/particles_100.bin
```


# Aide pour les courbes de scalabilité

Des scripts sont également à votre disposition pour les courbes de scalabilité :
- `weak_scaling_omp.py`
- `weak_scaling_mpi.py`
- `strong_scaling_omp.py`
- `strong_scaling_mpi.py`
