# 1. Architecture d'un super-calculateur

## Frontale, ordonnanceur et nœuds de calcul

--8<-- "super-calculateur/architecture.svg"

- Vous vous connectez d'abord sur la **frontale** (*login node*) : c'est
  un serveur **partagé** qui accueille les utilisateurs. Il permet de
  gérer son environnement et d'exécuter de petits scripts.
- Pour faire tourner un programme, on ne l'exécute pas directement comme
  sur son ordinateur : on le **soumet à un ordonnanceur** de tâches.
- Le rôle de l'ordonnanceur est de gérer l'allocation des ressources de
  calcul pour l'ensemble des utilisateurs, en respectant des règles de
  priorité et en maximisant le taux d'occupation du super-calculateur.
- L'ordonnanceur utilisé sur Ruche s'appelle **SLURM**.

!!! warning "Ne calculez pas sur la frontale"

    La frontale est partagée par tous les utilisateurs. Un calcul lourd
    lancé directement dessus la ralentit pour tout le monde : les calculs
    passent toujours par l'ordonnanceur.

## Les ressources de calcul de Ruche

<!-- TODO : vérifier que ces chiffres sont à jour -->

| Type de nœud | Nombre | Équipement |
|---|---|---|
| CPU | 216 | 2 processeurs Intel Cascade Lake de 20 cœurs chacun |
| Hybride GPU | 10 | GPU NVIDIA V100 |
| Hybride GPU | 9 | GPU NVIDIA A100 |

Pour des raisons d'économie d'énergie, tous les nœuds ne sont pas
forcément allumés.

!!! info "Dans ce cours"

    Nous n'utiliserons que les **nœuds CPU**.

## Les partitions

Les ressources de calcul sont en général divisées en **partitions**, qui
ont chacune leurs propriétés (durée maximale, nombre de nœuds, type de
matériel…). Les partitions CPU de Ruche :

| Partition | Usage |
|---|---|
| `cpu_short` | Jobs courts (1 h maximum). **C'est celle que nous utiliserons.** |
| `cpu_med` | Jobs de taille moyenne |
| `cpu_long` | Jobs longs |
| `cpu_prod` | Jobs de production |

Sur d'autres calculateurs, les partitions n'ont pas les mêmes noms ni les
mêmes fonctions, mais le principe reste le même.

La commande `sinfo` affiche les informations sur les partitions :

```bash
sinfo
```

:material-book-open-variant: [Description des partitions de Ruche](https://mesocentre.pages.centralesupelec.fr/user_doc/ruche/07_slurm_partitions_description/)

## Les espaces de stockage

Tous les super-calculateurs donnent accès à plusieurs **espaces de
stockage** (*file systems*), chacun avec un rôle précis :

| Espace | Rôle |
|---|---|
| `$HOME` | Espace par défaut à la connexion. Relativement petit (quelques Go), mais **sauvegardé**. |
| `$WORK` | Espace plus conséquent (souvent quelques To) pour installer les codes et les bibliothèques et stocker de gros fichiers. |
| `$SCRATCH` | Espace de travail des simulations, très rapide en lecture et en écriture, mais **non sauvegardé**. |
| `$STORE` | Sauvegarde de gros volumes de données à long terme ; l'accès est réservé aux gros fichiers. |

!!! info "Sur Ruche"

    Seuls deux espaces sont disponibles : `$HOME` (50 Go) et `$WORK`
    (500 Go).
