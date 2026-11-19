# 1. Architecture d'un super-calculateur

## Frontale, ordonnanceur et nœuds de calcul

<svg class="diagram diagram-wide" viewBox="0 0 760 320" role="img" aria-label="Les utilisateurs se connectent en SSH à la frontale, qui envoie les jobs à l'ordonnanceur ; celui-ci les répartit sur trois partitions de nœuds de calcul">
<defs><marker id="arrow-sc" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path class="arrowhead" d="M0,0 L10,5 L0,10 z"/></marker></defs>
<rect class="frame" x="200" y="8" width="552" height="304" rx="10"/>
<text class="small" x="476" y="26">Super-calculateur</text>
<rect class="bus" x="10" y="125" width="100" height="70" rx="8"/><text x="60" y="160">Utilisateurs</text>
<line class="link" x1="110" y1="160" x2="222" y2="160" marker-end="url(#arrow-sc)"/>
<text class="small" x="155" y="182">Connexion</text><text class="small" x="155" y="196">distante (SSH)</text>
<rect class="mem" x="225" y="105" width="110" height="110" rx="8"/><text x="280" y="152">Frontale</text><text class="small" x="280" y="170">(login node)</text>
<line class="link" x1="335" y1="160" x2="412" y2="160" marker-end="url(#arrow-sc)"/>
<text class="small" x="374" y="182">Envoi des</text><text class="small" x="374" y="196">travaux (job)</text>
<rect class="bus" x="415" y="125" width="110" height="70" rx="8"/><text x="470" y="152">Ordonnanceur</text><text class="small" x="470" y="170">(SLURM)</text>
<line class="link" x1="525" y1="160" x2="587" y2="80" marker-end="url(#arrow-sc)"/>
<line class="link" x1="525" y1="160" x2="587" y2="160" marker-end="url(#arrow-sc)"/>
<line class="link" x1="525" y1="160" x2="587" y2="240" marker-end="url(#arrow-sc)"/>
<g><rect class="cpu" x="590" y="45" width="145" height="70" rx="8"/><text x="662" y="67">Partition 1</text><rect class="node" x="608" y="84" width="16" height="16" rx="2"/><rect class="node" x="630" y="84" width="16" height="16" rx="2"/><rect class="node" x="652" y="84" width="16" height="16" rx="2"/><rect class="node" x="674" y="84" width="16" height="16" rx="2"/><rect class="node" x="696" y="84" width="16" height="16" rx="2"/></g>
<g><rect class="cpu" x="590" y="125" width="145" height="70" rx="8"/><text x="662" y="147">Partition 2</text><rect class="node" x="608" y="164" width="16" height="16" rx="2"/><rect class="node" x="630" y="164" width="16" height="16" rx="2"/><rect class="node" x="652" y="164" width="16" height="16" rx="2"/><rect class="node" x="674" y="164" width="16" height="16" rx="2"/><rect class="node" x="696" y="164" width="16" height="16" rx="2"/></g>
<g><rect class="cpu" x="590" y="205" width="145" height="70" rx="8"/><text x="662" y="227">Partition 3</text><rect class="node" x="608" y="244" width="16" height="16" rx="2"/><rect class="node" x="630" y="244" width="16" height="16" rx="2"/><rect class="node" x="652" y="244" width="16" height="16" rx="2"/><rect class="node" x="674" y="244" width="16" height="16" rx="2"/><rect class="node" x="696" y="244" width="16" height="16" rx="2"/></g>
<text class="small" x="662" y="295">Nœuds de calcul</text>
</svg>

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

## Testez-vous

??? question "Pourquoi ne lance-t-on pas un calcul directement sur la frontale ?"

    La frontale est un serveur partagé par tous les utilisateurs et ne
    dispose pas des ressources des nœuds de calcul. Les calculs doivent
    être soumis à l'ordonnanceur, qui leur attribue des nœuds de calcul.

??? question "Où écrire les résultats d'une grosse simulation sur un centre qui propose `$SCRATCH` ?"

    Dans `$SCRATCH`, qui est rapide en lecture et en écriture. Comme il
    n'est pas sauvegardé, on copie ensuite les résultats à conserver vers
    `$WORK` ou `$STORE`.
