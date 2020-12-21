# Analyse des fichiers de sortie

L'éxecution du code genère plusieurs type de fichiers dans un dossier `diags`
qui sera créé au début de chaque *run* :
- fichiers hdf5 (`.h5`)
- fichiers bianire (`.bin`)
- fichiers vtk pour Paraview (`.vtk`)
Les fichiers de sortie portent le nom suivant `particles_<numéro d'itération>.bin`.
Ce dossier contient des scripts Python dédiés à l'analyse des résultats.
Ils permettent d'ouvrir les fichiers de sortie pour visualiser les résultats :

- `plot_binary_matplotlib.py` : ce fichier permet de lire et d'afficher les données d'un fichier binaire (`.bin`) spécifié en ligne de commande. Il utilise la bibliothèque Matplotlib. Le rendu 3D n'est pas parfait mais ce script a l'avantage d'être utilisable avec quasiment tous les environnements Python. Ce script est d'ailleurs compatible python 2 et 3.
```bash
# Dans le dossier de simulation, cette commande affichera
# les particules à l'itération 100 si le fichier binaire existe.
python ../python/plot_binary_matplotlib.py diags/particles_100.bin
```

- `plot_binary_mayavi.py` : ce script permet de visualiser les fichiers binaires `.bin` en utilisant la bibliothèque d'affichage scientifique 3D [Mayavi](http://docs.enthought.com/mayavi/mayavi/). Ce script nécessite donc Mayavi et Python 3.
```bash
# Dans le dossier de simulation, cette commande affichera
# les particules à l'itération 100 si le fichier binaire existe.
python ../python/plot_binary_mayavi.py diags/particles_100.bin
```

- `plot_hdf5.py` : ce script permet de lire et d'afficher les données issues d'un fichier HDF5 (`.h5`). Il utilise également Matplotlib.
```bash
# Dans le dossier de simulation, cette commande affichera
# les particules à l'itération 100 si le fichier binaire existe.
python ../python/plot_hdf5.py diags/particles_100.h5
```
