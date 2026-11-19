# Site web du cours

Version web des supports de cours, construite avec
[MkDocs Material](https://squidfunk.github.io/mkdocs-material/) et publiée
sur GitHub Pages.

## Prévisualiser en local

```bash
cd website
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
mkdocs serve        # http://127.0.0.1:8000, rechargement automatique
```

## Organisation

```
website/
├── mkdocs.yml                 # configuration, menu de navigation
├── requirements.txt
└── docs/
    ├── index.md               # page d'accueil
    ├── introduction/          # chapitre I (une page par section)
    ├── assets/images/<chapitre>/
    ├── stylesheets/extra.css
    └── javascripts/mathjax.js
```

Ajouter un chapitre : créer `docs/<chapitre>/`, puis l'ajouter dans la
section `nav` de `mkdocs.yml`.

### Images partagées avec le reste du dépôt

Une image utilisée à la fois par le site et par un README du dépôt n'est
stockée qu'une fois, dans `website/docs/assets/images/`. Le README pointe
alors vers ce fichier (par exemple
`../website/docs/assets/images/super-calculateur/filezilla.jpg`) et la copie
de `support/materiel/` est supprimée.

### Syntaxe utile

- Maths : `\(x^2\)` en ligne, `\[ ... \]` en bloc.
- Encadrés : `!!! note "Titre"`, `!!! warning`, et `??? question "..."`
  pour une question dont la réponse est repliée.
- Onglets : `=== "Titre"`.
- Vidéos : pour l'instant remplacées par une capture (`poster-*.jpg`).
  Pour en réintégrer une, compresser puis utiliser une balise `<video>` :
  `ffmpeg -i in.mp4 -vf "scale='min(960,iw)':-2" -c:v libx264 -crf 30 -an -movflags +faststart out.mp4`

## Publication

Le workflow `.github/workflows/website.yml` construit et publie le site à
chaque push sur `master` qui modifie `website/`.

Une seule fois : dans *Settings → Pages* du dépôt GitHub, choisir
**Source : GitHub Actions**.

URL : <https://maison-de-la-simulation.github.io/HPC-DFE-Paris-Saclay/>

## Note

Material for MkDocs arrive en fin de vie en mai 2027. Son successeur,
[Zensical](https://zensical.org/), lit directement ce `mkdocs.yml`
(`pip install zensical && zensical serve`) : la migration ne demandera pas
de réécrire les pages.
