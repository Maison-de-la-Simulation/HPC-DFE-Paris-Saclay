# II. Prise en main d'un super-calculateur

Ce chapitre vous apprend à utiliser concrètement un super-calculateur :
s'y connecter, préparer son environnement et lancer des calculs via un
ordonnanceur de tâches.

## Le mésocentre de l'Université Paris-Saclay

Le [mésocentre de l'Université Paris-Saclay](https://mesocentre.universite-paris-saclay.fr/)
regroupe trois plateformes techniques majeures de calcul scientifique et
de traitement de données, issues des établissements partenaires du projet
Paris-Saclay. Ces plateformes sont accessibles aux chercheurs **et aux
étudiants**.

Pour ce cours, nous utiliserons le super-calculateur **Ruche** du
mésocentre. Son fonctionnement est très similaire à celui de la plupart
des super-calculateurs du monde, même les plus gros : ce que vous
apprendrez ici vous servira ailleurs.

:material-book-open-variant: [Documentation utilisateur de Ruche](https://mesocentre.pages.centralesupelec.fr/user_doc/)

!!! danger "Conditions d'utilisation"

    L'utilisation des ressources du mésocentre est **strictement limitée à
    un usage en lien avec le Master**. Vous pouvez l'utiliser pour les
    autres matières, mais en aucun cas pour des usages personnels ou
    professionnels.

## Objectifs

À la fin de ce chapitre, vous saurez :

- décrire l'architecture d'un super-calculateur : frontale, nœuds de
  calcul, partitions, espaces de stockage ;
- vous connecter à Ruche en SSH, transférer des fichiers et éditer du code ;
- gérer votre environnement logiciel avec les **modules** ;
- écrire un script **SLURM**, le soumettre et suivre son exécution.

## Plan du chapitre

<div class="grid cards" markdown>

-   __1. Architecture__

    ---

    Frontale, ordonnanceur, ressources de Ruche, partitions et espaces
    de stockage.

    [:octicons-arrow-right-24: Lire](architecture.md)

-   __2. Connexion et environnement__

    ---

    SSH, quotas, éditeurs, transfert de fichiers et modules.
    **Exercice 1.**

    [:octicons-arrow-right-24: Lire](environnement.md)

-   __3. SLURM et premier job__

    ---

    Scripts SLURM, soumission et suivi des jobs.
    **Exercice 2.**

    [:octicons-arrow-right-24: Lire](slurm.md)

</div>
