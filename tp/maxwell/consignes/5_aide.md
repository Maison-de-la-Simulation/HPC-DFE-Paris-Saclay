# Aide et conseils

# Questions posées

1. Les valeurs locales nxp, nyd, xminp et xmind à calculer et à déclarer (dans la question 3.5.c) ne sont-elles pas déja déclarer dans la section " Initilization of arrays and internal parameters " du code ?

Elles sont calculées dans la version séquentielle pour tout le domaine. Dans la version MPI, il faut créer une version locale de ces variables propres au domaine MPI. Par exemple :
- `nxp` qui correspond au nombre de cellules primales dans la direction x pour chaque sous-domaine MPI
- `xminp` la borne inférieure en x de la grille primale
- etc

Cela permet d'avoir dea valeurs locales à chaque sous-domaine.

Voici un exemple de ce que j'ai fait pour la grille duale :


```C++

    int nxd_global = nx;
    int nxp_global = nx-1;

    // Dual
    int nxd = nx / number_of_ranks;
    const int nyd = ny;

    if (rank_neighbors_mx >= 0) {
        nxd += 1;
    }

    if (rank_neighbors_px >= 0) {
        nxd += 1;
    }
```

Ici j'ai déclaré des variables pour tout le domaine `nxd_global` et `nxp_global`. Les autres variables deviennent locales à chaque sous-domaine d'où la division par `number_of_ranks` et un traitement spécial pour les bords.


2) Faut-il commenter les lignes de la section "Initial diagnostics output" et "Diagnostics output" ? Dans la correction des anciens TP, ces lignes ne figurent pas ou sont commentés, lorsque ces lignes ne sont pas commentés le processus MPI s'arrête mais quand je les commente, j'obtiens enfin un résultat que j'ai mis en pièce jointe.

Oui car ces fonctions ne sont pas encore prêtes pour MPI. Dans le pire des cas ça crash mais dans le meilleures des cas vous avez un résultat faux.

3) Les conditions limites réfléchissantes aux bord du domaine à mettre à jour (3.5.g) correspondent elles bien au bords - X, +X, -Y, +Y ? Si c'est le cas, elles ne sont pas déjà mises à jour dans la boucle en temps (dans la section "Apply boundary conditions (Reflective)" ?

C'est exact mais il ne faut les appliquer que pour les sous-domaines MPI qui sont aux bords. Si vous ne changez rien, chaque sous-domaine applique les conditions limites comme s'il était sans voisin. Ici on ne doit faire une modification que pour -X et +X car on ne divise le domaine que dans cette direction.

Voici un exemple de comment les gérer pour -X:

```C++
        // Apply boundary conditions (Reflective)
        // -------------------------------------

        // MPI domain at the -X boundary
        if (rank_neighbors_mx < 0) {

            // Bord -X (ix = 0)
            for (int iy = 0 ; iy < nyp ; iy++) {
                By[iy] = By[iy + nyp];
            }
            for (int iy = 1 ; iy < nyd-1 ; iy++) {
                Bz[iy] = Bz[iy + nyd];
            }

        }
```
