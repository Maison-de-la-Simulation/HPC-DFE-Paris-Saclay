# Simulation des équations de Maxwell via la méthode aux différences finis explicite centrée de Yee

Ce TP s'intéresse à la modélisation des équations de Maxwell par la méthode aux différences finies de Yee.

## Description du répertoire

Ce TP se compose de plusieurs répertoires :
- [consignes](./consignes) : ce dossier contient les consignes du TP
- [cpp/sequential](./cpp/sequential) : ce dossier contient le fichier source du code séquentiel et le makefile pour le compiler.
- [visualization](./python) : ce dossier contient des scripts dédiés à la visualisation des résultats.
- [scalings](./python) : ce dossier contient des scripts permettant d'afficher les courbes de scalabilité
- [.extra](./.extra) : ce dossier sert uniquement pour GitHub

## Présentation du modèle

Nous nous intéressons dans ce projet à la simulation des équations Maxwell par l'utilisation d'une méthode aux différences finies très connue du nom de FDTD pour Finite-Difference Time-Domain publié par K. Yee dans les années 60 et toujours utilisée aujourd'hui dans les codes de calcul pour sa rapidité et sa simplicité.

On rappelle les équations de Maxwell dans le vide :

Maxwell Ampere :

$$
\frac{1}{c^2} \frac{\partial E}{\partial t}  = - \mu_0 \vec{J} + \nabla \times \vec{B}
$$

Maxwell Faraday :

$$
\frac{\partial B}{\partial t} = - \nabla \times \vec{E}
$$

Maxwell Gauss :

$$
\nabla \cdot \vec{E} = 0
$$

Maxwell Flux :

$$
\nabla \cdot \vec{B} = 0
$$

Dans le cadre de la discrétisation de Yee, on ne s'intéresse qu'à Maxwell Ampere et Faraday. On commence par normaliser les équations pour éliminer les constantes et réduire l'amplitude des champs (ce qui est toujours préférable en informatique).

$$
\frac{\partial E}{\partial t} = - \vec{J} + \nabla \times vec{B}
\frac{\partial B}{\partial t} = - \nabla \times vec{E}
$$

En 2D, la discrétisation donne les équations suivantes pour Maxwell Ampère :
        
$$
Ex_{i,j}^{n} = Ex_{i,j}^{n-1} + \frac{\Delta t}{\Delta y} \left( Bz_{i,j+1}^{n-1/2}  - Bz_{i,j}^{n-1/2}\right)
$$

$$
Ey_{i,j}^{n} = Ey_{i,j}^{n-1} + \frac{\Delta t}{\Delta x} \left( Bz_{i+1,j}^{n-1/2}  - Bz_{i,j}^{n-1/2}\right)
$$

$$
Ez_{i,j}^{n} = Ez_{i,j}^{n-1} + \frac{\Delta t}{\Delta x} \left( By_{i+1,j}^{n-1/2}  - By_{i,j}^{n-1/2}\right) - \frac{\Delta t}{\Delta y} \left( Bx_{i,j+1}^{n-1/2}  - Bx_{i,j}^{n-1/2}\right) 
$$

Où $n = t / \Delta t $ est l'iteration en temps, $i$ et $j$ respectivement la discrétisation en espace suivant les directions $x$ et $y$.

Puis pour Maxwell Faraday :

$$
Bx_{i,j}^{n-1/2} = Bx_{i,j}^{n+1/2}  - \frac{\Delta t}{\Delta y} \left( Ez_{i,j}^{n}  - Ez_{i,j-1}^{n}\right) 
$$

$$
By_{i,j}^{n-1/2} = By_{i,j}^{n+1/2}  + \frac{\Delta t}{\Delta x} \left( Ez_{i,j}^{n}  - Ez_{i-1,j}^{n}\right) 
$$

$$
Bz_{i,j}^{n-1/2} = Bz_{i,j}^{n+1/2}  + \frac{\Delta t}{\Delta y} \left( Ex_{i,j}^{n}  - Ex_{i,j-1}^{n}\right) 
                                     - \frac{\Delta t}{\Delta x} \left( Ey_{i,j}^{n}  - Ey_{i-1,j}^{n}\right) 
$$

Le modèle discrétisé peut être représenté par le schéma suivant :

<img src="../../support/materiel/maxwell_yee_grid.png" height="400">

Ici, les composantes de champ sont représentées par des noeuds colorés :

- rouge : $B_z$
- vert : $Ex$ et $By$
- orange : $Ez$
- bleu : $Ey$ et $Bx$

On remarque que les grilles qui composent les noeuds ne sont pas donc pas toutes superposées mais sont décalées en espace de $0.5 \Delta x$ et/ou $0.5 \Delta y$. En anglais, on parle de grille *staggered*. En 2D, certaines composantes existent à la même position mais ce n'est pas le cas en 3D.
Les champs électriques et magnétiques sont également décalés en temps de $0.5 \Delta t$.

La grille représentée par les noeuds rouge du champ $Bz$ est appelée grille duale.
C'est la grille la plus large.

Dans la figure ci-dessous, nous avons représenté la grille définie par les noeuds oranges du champ $E_z$.
il s'agit de la grille primale.

<img src="../../support/materiel/maxwell_yee_grid_3.png" height="400">

Le grille primale est celle qui contient le moins de noeuds.
Par définition, si la grille primale a une taille de $n_{xp} \times n_{yp}$ alors la grille duale a une taille de $n_{xd} = n_{xp} + 1 \times n_{yd} = n_{yp} + 1$.

Toutes les autres composantes sont une combinaison dual / primal.
Par exemple, la grille définie par les noeuds bleus (composantes $Ey$ et $Bx$) est primale dans la direction $x$ mais duale dans la direction $y$. elle a donc une taille $n_{xp} \times n_{yd}$.
De la même manière, la grille définie par les noeuds verts (composante $Ex$ et $By$) est au contraire duale dans la direction $x$ et primale dans la direction $y$.
La figure ci-dssous résume la notion de grille primale et duale :

<img src="../../support/materiel/maxwell_primal_dual.png" height="400">

## Conditions limites

Il faut maintenant se pencher sur les conditions limites.
Les composantes de champs qui nécessitent d'être traitées sont aux bords sont situées sur la ligne violette de la figure ci-dessous :

<img src="../../support/materiel/maxwell_yee_grid_2.png" height="400">

De plus, les équations nous disent que les inconnus aux bords sont toutes des composantes du champ magnétique.
En effet les champs électriques aux bords se calculent à partir des champs magnétiques connus.

## Notion d'antenne

Une façon de générer un champ électromagnétiest est d'émuler le comportement d'une antenne.
Une antenne peut être modélisée par une charge qui se déplace en oscillant le long d'un segment.
Il faut alors calculer le courant généré par cette antenne et l'ajouter aux équations de Maxell-Ampère.

Soit $\vec{x}_A(t)$ l'évolution dans l'espace d'une charge $q_A$ et donc $\vec{v}_A(t)$ sa vitesse.
Le courant est donné par :

$$
\vec{J}_A = q_A \vec{v}_A(t)
$$

Pour une antenne, on utilise :

$$
\vec{x}_A = \vec{x}_{A,0} + L_{A} \cos{ \left( 2 \pi t / T_{A} \right) }
$$

Avec $L_A$ la longueur de l'antenne et $T_A$ sa période.


