# Analyse des fichiers de sortie

## Scripts Python

L’exécution du code génère plusieurs type de fichiers dans un dossier `diags`
qui sera créé au début de chaque *run* :
- fichier binaire représentant le contenu des grilles à une itération donnée (`diag_*.bin`).
- fichier ascii représentant le contenu des grilles à une itération donnée (`diag_*.dat`).
- fichier binaire contenant l'évolution en temps de certaines grandeurs réduites (du nom de `scalars.bin`)

Les fichiers de sortie portent le nom suivant `diag_<numéro d'itération>.bin`.
Ce dossier contient des scripts Python dédiés à l'analyse des résultats.
Ils permettent d'ouvrir les fichiers de sortie pour visualiser les résultats :
