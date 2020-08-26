# Cas de l'équation d'onde par la méthode des différences finies

Ce cas applicatif s'intéresse à la parallèlisation d'un code résolvant
l'équation d'onde 2D avec la méthode des différences finies.

L'équation d'onde que nous souhaitons résoudre correspond à l'équation dans un milieu homogène et isotrope :
$$
\frac{\partial^2 U(\mathbf{x},t)}{\partial t ^2} = c^2 \Delta U(\mathbf{x},t) + f
$$
Où $c$ est la vitesse de propagation des ondes dans le milieu dans toutes les directions (isotropie).
Le terme $f$ est ici un terme source.

En 2D, cette équation devient :
$$
\frac{\partial^2 U(\mathbf{x},t)}{\partial t^2} = c^2 \cdot \left[ \frac{\partial^2 U(\mathbf{x},t)}{\partial x^2}  + \frac{\partial^2 U(\mathbf{x},t)}{\partial y^2}\right] + f
$$

Dans cette exemple, nous utilisons des conditions limites de type Dirichlet.
$$
U(\mathbf{x},t)_{boundary} = 0
$$
Cela correspond à une réflection parfaite de l'onde sur les bords du domaine.
On peut noter qu'une condition de type Neumann nulle correspondrait à une absorption parfaite.

La discrétisation par la méthode des différences finies donne:
```math
U^{n+1}_{i,j} = 2 U^{n}_{i,j} - U^{n-1}_{i,j} + D  \cdot \left( U^{n}_{i+1,j} + U^{n}_{i-1,j} - 4 U^{n}_{i,j} + U^{n}_{i,j+1} + U^{n}_{i,j-1}  \right) + \Delta t ^2 f
```
où $`D`$ est un coefficient valant $`\left( c \Delta t / \Delta x \right)^2`$

Ici, $`\Delta t`$ est le pas de temps. On a donc $`t = n \cdot \Delta t `$. Il faut alors comprendre que pour obtenir le temps $`n+1`$
nous avons besoin des deux pas de temps précédent.
$`\Delta x`$ représentent le pas d'espace à la fois dans les directions $`x`$ et $`y`$.
Les indices $`i`$ et $`j`$ correspondent aux positions suivant $`x`$ et $`y`$
sur la grille 2D de telle sorte que $`x = i \cdot \Delta x`$ et $`y = j \cdot \Delta x`$.
De la même façon, $`\Delta t`$ est le pas de temps de telle sorte qu'en
définissant l'itération $`k`$, nous obtenons $`t = k \cdot \Delta t`$.

La condition de stabilité du schéma est donnée par la formule :
```math
dt \leq dx / c
```

Les implémentations présentées dans ce dossier respectent ce formalisme.

## Implémentations

L'équation 2D est implémentée dans le code séquentiel fortran situé dans le dossier [applications/onde/fortran](./fortran/).
Les instructions de compilation et d'éxecution du code sont disponibles sur ce [README](./fortran/README.md).

## Analyse des résultats

L'éxecution du code genère des fichiers de sortie binaires dans un dossier `diags`
qui sera créé au début de chaque *run*.
Les fichiers de sortie portent le nom suivant `grid_<numéro d'itération>.dat`.
Le dossier [applications/onde/python](./python/`) contient des scripts Python
dédiés à l'analyse des résultats.
Les instructions pour utiliser ces scripts sont disponibles sur le [README](./python/README.md) correspondant.
