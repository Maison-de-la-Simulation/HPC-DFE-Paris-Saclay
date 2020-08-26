# Cas de l'équation de diffusion linéaire

Ce cas applicatif s'intéresse à la parallèlisation d'un code résolvant
l'équation de diffusion linéaire 2D avec la méthode des différences finies.

L'équation de diffusion linéaire correspond à l'équation suivante :
```math
\frac{\partial U(\mathbf{x},t)}{\partial t} = D \nabla ^2 U(\mathbf{x},t)
```
Où $`D`$ est le coefficient de diffusion dans toutes les directions.

En 2D, cette équation devient :
```math
\frac{\partial U(\mathbf{x},t)}{\partial t} = D \cdot \left[ \frac{\partial^2 U(\mathbf{x},t)}{\partial x^2}  + \frac{\partial^2 U(\mathbf{x},t)}{\partial y^2}\right]
```

Dans cette exemple, nous utilisons des conditions limites de type Dirichlet.
```math
U(\mathbf{x},t)_{boundary} = 0
```

La discrétisation par la méthode des différences finies donne:
```math
U^{t+\Delta t}_{i,j} = U^{t}_{i,j} + C_x  \cdot \left( U^{t}_{i+1,j} - 2 U^{t}_{i,j} + U^{t}_{i-1,j} \right)
+ C_y  \cdot \left( U^{t}_{i,j+1} - 2 U^{t}_{i,j} + U^{t}_{i,j-1} \right)
```

Dans cette formulation, $`\Delta x`$ et $`\Delta y`$ représentent respectivement
les pas d'espace dans les directions $`x`$ et $`y`$.
Les indices $`i`$ et $`j`$ correspondent aux positions suivant $`x`$ et $`y`$
sur la grille 2D de telle sorte que $`x = i \cdot \Delta x`$ et $`y = j \cdot \Delta y`$.
De la même façon, $`\Delta t`$ est le pas de temps de telle sorte qu'en
définissant l'itération $`k`$, nous obtenons $`t = k \cdot \Delta t`$.

Les coefficients $`C x`$ et $`C y`$ sont donnés par les formules :
```math
C_x =\frac{D \Delta t }{\Delta x^2} ; C_y =\frac{D \Delta t }{\Delta y^2}
```

La condition de stabilité du schéma est donnée par la formule :
```math
C_x + C_y \leq \frac{1}{2}
```

Les implémentations présentées dans ce dossier respectent ce formalisme.


## Implémentations

L'équation 2D est implémentée dans le code fortran situé dans le dossier [applications/diffusion/fortran](./fortran/).
Le code utilise une parallèlisation MPI.
Les instructions de compilation et d'éxecution du code sont disponibles sur ce [README](./fortran/README.md).

## Analyse des résultats

L'éxecution du code genère des fichiers de sortie HDF5 dans un dossier `diags`
qui sera créé au début de chaque *run*.
Les fichiers de sortie portent le nom suivant `grid_<numéro d'itération>.h5`.
Le dossier [`applications/diffusion/python`](./python/`) contient des scripts Python
dédiés à l'analyse des résultats.
Les instructions pour utiliser ces scripts sont disponibles sur le [README](./python/README.md) correspondant.

# Consignes

Elles sont dans le [README](./fortran/README.md) du code.
