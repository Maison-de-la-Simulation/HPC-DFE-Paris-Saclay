# 3. Notion de programme parallèle

Un super-calculateur est une machine parallèle : il rassemble des milliers
de cœurs de calcul, répartis sur de nombreux nœuds (nous verrons comment
dans la partie suivante,
[Fonctionnement d'un super-calculateur](fonctionnement.md)). Encore
faut-il que le programme sache les utiliser. Cette partie présente la
différence entre un programme **séquentiel** et un programme
**parallèle**.

## Le calcul séquentiel

Un programme **séquentiel** est une suite d'instructions exécutées **l'une
après l'autre**, par **une seule unité de calcul**. C'est ainsi que l'on
écrit naturellement un programme : chaque instruction commence quand la
précédente est terminée.

Prenons la somme des nombres de 1 à 16. En séquentiel, une seule unité de
calcul les additionne un par un :

--8<-- "introduction/somme-sequentielle.svg"

Quel que soit le nombre de cœurs de la machine, un programme séquentiel
n'en utilise qu'**un seul**. Pour aller plus vite, il ne peut compter que
sur la vitesse de ce cœur.

## Le calcul parallèle

En calcul **parallèle**, plusieurs unités de calcul travaillent **en même
temps**. Il y a deux grandes façons de leur donner du travail.

### Découper un même travail : le parallélisme de données

On découpe **un même travail** en morceaux, et chaque unité effectue **le
même calcul** sur son morceau de données.

!!! example "Une analogie : corriger des copies"

    Un enseignant seul corrige 100 copies l'une après l'autre : c'est du
    séquentiel. Avec 4 correcteurs qui se partagent le paquet, chacun
    corrige 25 copies en même temps que les autres : c'est du parallèle.
    Il faut toutefois répartir les copies au départ et rassembler les notes
    à la fin.

Reprenons la somme des nombres de 1 à 16. En parallèle, on découpe la
liste en 4 morceaux : chaque unité calcule la somme de son morceau, puis
on combine les 4 **sommes partielles**.

--8<-- "introduction/somme-parallele.svg"

C'est la forme de parallélisme la plus courante en simulation : on découpe
le domaine de calcul (une grille, un maillage) entre les unités.

### Répartir des tâches différentes : le parallélisme de tâches

On peut aussi confier à chaque unité **une tâche différente**. Les tâches
s'exécutent en même temps et échangent des données quand elles en ont
besoin.

!!! example "Une analogie : la cuisine d'un restaurant"

    Pour servir un repas, un cuisinier prépare l'entrée, un autre le plat
    et un troisième le dessert, tous en même temps. Chacun fait un travail
    différent, mais ils doivent se coordonner pour que tout soit prêt au
    bon moment.

Les **modèles de climat** en sont un exemple : la simulation de
l'atmosphère et celle de l'océan sont deux codes différents, qui tournent
en même temps sur des unités de calcul différentes et échangent
régulièrement des données (températures, flux de chaleur…). D'autres
unités peuvent, pendant ce temps, analyser et écrire les résultats.

--8<-- "introduction/parallelisme-taches.svg"

!!! note "Les deux se combinent"

    Dans un modèle de climat, chaque tâche est elle-même parallélisée :
    l'atmosphère est découpée entre de nombreuses unités de calcul, et
    l'océan entre d'autres. Les grands codes de simulation mélangent
    souvent les deux formes de parallélisme.

## Ce que l'on gagne, et ses limites

Idéalement, avec \(N\) unités de calcul, le calcul va \(N\) fois plus
vite. En pratique, le gain est plus faible :

- **Une partie reste séquentielle** : dans l'exemple, la combinaison des
  sommes partielles ; dans un vrai code, la lecture des données ou
  l'écriture des résultats.
- **Les unités doivent échanger des données et se synchroniser** : ces
  communications prennent du temps, pendant lequel on ne calcule pas.
- **La charge peut être mal répartie** : si une unité a plus de travail
  que les autres, celles-ci l'attendent.
- **Tout ne se découpe pas** : certaines étapes dépendent du résultat des
  précédentes. Dans une simulation, le pas de temps \(t + 1\) a besoin du
  résultat du pas \(t\) : on ne peut pas calculer les pas de temps en même
  temps. On parallélise alors **à l'intérieur** de chaque pas de temps, en
  découpant le domaine de calcul entre les unités.

Ces notions seront mesurées en détail dans la partie sur la mesure de la
performance.

!!! info "À retenir"

    Un super-calculateur n'accélère un calcul que si le programme est
    **parallèle**. Lancé sur un super-calculateur, un programme séquentiel
    n'utilise qu'un seul de ses milliers de cœurs.
