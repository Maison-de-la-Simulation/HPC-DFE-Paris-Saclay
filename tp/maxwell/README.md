# Simulation des équations de Maxwell via la méthode aux différences finis explicite centrée de Yee

Ce TP s'intéresse à la modélisation des équations de Maxwell par la méthode aux différences finies de Yee.

## Table des matières

1. [Description du répertoire](#description) 
2. [Présentation du modèle](#modele)
3. [Le code séquentiel](#code_sequentiel)
4. [Consignes](#consignes)

## Description du répertoire <a id="description"></a>

Ce TP se compose de plusieurs répertoires :
- [consignes](./consignes) : ce dossier contient les consignes du TP
- [cpp/sequential](./cpp/sequential) : ce dossier contient le fichier source du code séquentiel et le makefile pour le compiler.
- [visualization](./python) : ce dossier contient des scripts dédiés à la visualisation des résultats.
- [scalings](./python) : ce dossier contient des scripts permettant d'afficher les courbes de scalabilité
- [.extra](./.extra) : ce dossier sert uniquement pour GitHub

## Présentation du modèle <a id="modele"></a>

### Schéma de Yee

Nous nous intéressons dans ce projet à la simulation des équations Maxwell par l'utilisation d'une méthode aux différences finies très connue du nom de FDTD pour Finite-Difference Time-Domain publié par K. Yee dans les années 60 et toujours utilisée aujourd'hui dans les codes de calcul pour sa rapidité et sa simplicité.

On rappelle les équations de Maxwell dans le vide :

Maxwell Ampere :

<p align="center"><img src=".extra//2ba4806c4179b91768ae5de6aa4f95f1.svg?invert_in_darkmode" align=middle width=173.07893955pt height=33.81208709999999pt/></p>

Maxwell Faraday :

<p align="center"><img src=".extra//6d5dd1abd04119bbe000966bdb8b84f3.svg?invert_in_darkmode" align=middle width=106.68419024999999pt height=33.81208709999999pt/></p>

Maxwell Gauss :

<p align="center"><img src=".extra//ac35e7e2e9828c6c627824cedac3fda7.svg?invert_in_darkmode" align=middle width=68.7896517pt height=15.89952705pt/></p>

Maxwell Flux :

<p align="center"><img src=".extra//9f912dfbf42cb7130565c37d471ce31d.svg?invert_in_darkmode" align=middle width=69.00087479999999pt height=15.89952705pt/></p>

Dans le cadre de la discrétisation de Yee, on ne s'intéresse qu'à Maxwell Ampere et Faraday. On commence par normaliser les équations pour éliminer les constantes et réduire l'amplitude des champs (ce qui est toujours préférable en informatique).

<p align="center"><img src=".extra//41cccb13579195958c31b4b2e6314e52.svg?invert_in_darkmode" align=middle width=292.37469525pt height=33.81208709999999pt/></p>

En 2D, la discrétisation donne les équations suivantes pour Maxwell Ampère :
        
<p align="center"><img src=".extra//ad7015d0a9a6cf54e178878906df03c7.svg?invert_in_darkmode" align=middle width=318.42411315pt height=36.82577085pt/></p>

<p align="center"><img src=".extra//c8e58c2c82b57b667d40e0b52121fa3b.svg?invert_in_darkmode" align=middle width=317.08854045pt height=33.62942055pt/></p>

<p align="center"><img src=".extra//11eb95a1f61dfd6015539c701060ebef.svg?invert_in_darkmode" align=middle width=529.30628685pt height=36.82577085pt/></p>

Où <img src=".extra//c461456e376ee5b83866a0c882334b0f.svg?invert_in_darkmode" align=middle width=65.57458379999998pt height=24.65753399999998pt/> est l'iteration en temps, <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> et <img src=".extra//36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/> respectivement la discrétisation en espace suivant les directions <img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> et <img src=".extra//deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649225749999989pt height=14.15524440000002pt/>.

Puis pour Maxwell Faraday :

<p align="center"><img src=".extra//c9127617f1f76d611abaeccae3fdbbee.svg?invert_in_darkmode" align=middle width=318.7628367pt height=36.82577085pt/></p>

<p align="center"><img src=".extra//256790f70fd976bd4fac7e4d3dce1460.svg?invert_in_darkmode" align=middle width=318.01703834999995pt height=33.62942055pt/></p>

<p align="center"><img src=".extra//ada59b372a869c3c1efb946ff3dbe61b.svg?invert_in_darkmode" align=middle width=495.31067025pt height=36.82577085pt/></p>

Le modèle discrétisé peut être représenté par le schéma suivant :

<img src="../../support/materiel/maxwell_yee_grid.png" height="400">

Ici, les composantes de champ sont représentées par des noeuds colorés :

- rouge : <img src=".extra//a4d087895430764308affdb5cdb32ffe.svg?invert_in_darkmode" align=middle width=19.221008399999988pt height=22.465723500000017pt/>
- vert : <img src=".extra//b7a981c903e3370fc32d1c5ee077472f.svg?invert_in_darkmode" align=middle width=22.47716459999999pt height=22.465723500000017pt/> et <img src=".extra//fcda124f6906a9960dc2e7b5b3c05029.svg?invert_in_darkmode" align=middle width=21.94262894999999pt height=22.465723500000017pt/>
- orange : <img src=".extra//bf80349ccc773775c05ac2c3c1e7fa2f.svg?invert_in_darkmode" align=middle width=21.44979869999999pt height=22.465723500000017pt/>
- bleu : <img src=".extra//a896e653d3de6b3985bf932a92e194b7.svg?invert_in_darkmode" align=middle width=21.73140254999999pt height=22.465723500000017pt/> et <img src=".extra//13119eef6557213b4399d373183db140.svg?invert_in_darkmode" align=middle width=22.68839099999999pt height=22.465723500000017pt/>

On remarque que les grilles qui composent les noeuds ne sont pas donc pas toutes superposées mais sont décalées en espace de <img src=".extra//e480030e45dcbe38c4de81952c8f8d73.svg?invert_in_darkmode" align=middle width=44.09830259999999pt height=22.465723500000017pt/> et/ou <img src=".extra//6a9471360fd71e276c860a8fffe83b7a.svg?invert_in_darkmode" align=middle width=43.352540549999986pt height=22.465723500000017pt/>. En anglais, on parle de grille *staggered*. En 2D, certaines composantes existent à la même position mais ce n'est pas le cas en 3D.
Les champs électriques et magnétiques sont également décalés en temps de <img src=".extra//0a329f8795cb025fc8778306028ae6aa.svg?invert_in_darkmode" align=middle width=40.63941254999999pt height=22.465723500000017pt/>.

La grille représentée par les noeuds rouge du champ <img src=".extra//e52d2c1ed64b7a4dd504cc2f505810cb.svg?invert_in_darkmode" align=middle width=21.66102509999999pt height=22.465723500000017pt/> est appelée grille duale.
C'est la grille la plus large.

Dans la figure ci-dessous, nous avons représenté la grille définie par les noeuds oranges du champ <img src=".extra//b2100207f858f063bea374511472fccd.svg?invert_in_darkmode" align=middle width=18.88707644999999pt height=22.465723500000017pt/>.
il s'agit de la grille primale.

<img src="../../support/materiel/maxwell_yee_grid_3.png" height="400">

Le grille primale est celle qui contient le moins de noeuds.
Par définition, si la grille primale a une taille de <img src=".extra//272af74b9dbbb880bbf4e3ffa9fc64ad.svg?invert_in_darkmode" align=middle width=68.73375134999998pt height=19.1781018pt/> alors la grille duale a une taille de <img src=".extra//102564694b48b1e313920ae04093e294.svg?invert_in_darkmode" align=middle width=219.60936855pt height=21.18721440000001pt/>.

Toutes les autres composantes sont une combinaison dual / primal.
Par exemple, la grille définie par les noeuds bleus (composantes <img src=".extra//a896e653d3de6b3985bf932a92e194b7.svg?invert_in_darkmode" align=middle width=21.73140254999999pt height=22.465723500000017pt/> et <img src=".extra//13119eef6557213b4399d373183db140.svg?invert_in_darkmode" align=middle width=22.68839099999999pt height=22.465723500000017pt/>) est primale dans la direction <img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> mais duale dans la direction <img src=".extra//deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649225749999989pt height=14.15524440000002pt/>. elle a donc une taille <img src=".extra//1eca62f8948ef6841d35298541aa46e5.svg?invert_in_darkmode" align=middle width=68.80035194999999pt height=19.1781018pt/>.
De la même manière, la grille définie par les noeuds verts (composante <img src=".extra//b7a981c903e3370fc32d1c5ee077472f.svg?invert_in_darkmode" align=middle width=22.47716459999999pt height=22.465723500000017pt/> et <img src=".extra//fcda124f6906a9960dc2e7b5b3c05029.svg?invert_in_darkmode" align=middle width=21.94262894999999pt height=22.465723500000017pt/>) est au contraire duale dans la direction <img src=".extra//332cc365a4987aacce0ead01b8bdcc0b.svg?invert_in_darkmode" align=middle width=9.39498779999999pt height=14.15524440000002pt/> et primale dans la direction <img src=".extra//deceeaf6940a8c7a5a02373728002b0f.svg?invert_in_darkmode" align=middle width=8.649225749999989pt height=14.15524440000002pt/>.
La figure ci-dssous résume la notion de grille primale et duale :

<img src="../../support/materiel/maxwell_primal_dual.png" height="400">

### Conditions limites

Il faut maintenant se pencher sur les conditions limites.
Les composantes de champs qui nécessitent d'être traitées sont aux bords sont situées sur la ligne violette de la figure ci-dessous :

<img src="../../support/materiel/maxwell_yee_grid_2.png" height="400">

De plus, les équations nous disent que les inconnus aux bords sont toutes des composantes du champ magnétique.
En effet les champs électriques aux bords se calculent à partir des champs magnétiques connus.

Dans le code, on utilise des conditions réfléchissantes (ou conditions de Neumann).

### Notion d'antenne

Une façon de générer un champ électromagnétiest est d'émuler le comportement d'une antenne.
Une antenne peut être modélisée par une charge qui se déplace en oscillant le long d'un segment.
Il faut alors calculer le courant généré par cette antenne et l'ajouter aux équations de Maxell-Ampère.

Soit <img src=".extra//2c09d6fe231fc69cfcee32fa0b3785df.svg?invert_in_darkmode" align=middle width=39.04402919999999pt height=24.65753399999998pt/> l'évolution dans l'espace d'une charge <img src=".extra//46d29bba147ab86127a70278268c42d9.svg?invert_in_darkmode" align=middle width=17.224222949999987pt height=14.15524440000002pt/> et donc <img src=".extra//e48500a898443f79affbb15aa1ea1eb7.svg?invert_in_darkmode" align=middle width=38.03569605pt height=24.65753399999998pt/> sa vitesse.
Le courant est donné par :

<p align="center"><img src=".extra//34f0431fabc6542e6237b3d2d33f1d61.svg?invert_in_darkmode" align=middle width=97.1842344pt height=20.00911605pt/></p>

Pour une antenne, on utilise :

<p align="center"><img src=".extra//7f9736b9470faf8a38e964e9a11db998.svg?invert_in_darkmode" align=middle width=207.70829475pt height=17.031940199999998pt/></p>

Avec <img src=".extra//71d5328dcc516b999b4b4630d795dc3f.svg?invert_in_darkmode" align=middle width=21.07315319999999pt height=22.465723500000017pt/> la longueur de l'antenne et <img src=".extra//a68349737baf72a520a69e2b4aac8d32.svg?invert_in_darkmode" align=middle width=19.49209844999999pt height=22.465723500000017pt/> sa période.


## Le code séquentiel <a id="code_sequentiel"></a>

Le code est écrit en langage C++ mais n'utilise pas de foctionnalité avancée.
Il ne se compose que d'un unique fichier `main.cpp`.

### Description du code

Le code se compose d'une phase d'initialisation puis d'une boucle ne temps (*main loop*) :

```c++
for (iteration = 1 ; iteration <= iterations ; iteration++) {
    //...
}
```

Dans la partie d'initialisation, on fait dans l'ordre :
- la déclaration des paramètres
- l'initialisation par défaut
- la lecture des arguments en ligne de commande
- l'initialisation des tableaux et des variables internes
- sortie des grilles dans des fichiers au temps 0

Puis dans la boucle en temps :
- résolution des équations de Maxwell-Ampère
- ajout des courants géénrés par les antennes
- résolution des équantions de Maxwell-Faraday
- conditions aux bords
- sortie des fichiers de diagntostique (pour chaque grille)
- affichage dans le terminal de l'état de la simulation

Puis après la boucle en temps :
- l'affichage des *timers*, c'est à dire le temps passé dans le divers parties du code
- destruction des tableaux

### Les dépendances

Ce programme nécessite l'installation d'un compilateur C++.

Vous n'avez besoin que d'une bibliothèque MPI.
Sur les postes de travail de l'université, OpenMPI est déjà installé.
Pour l'installer sur vos ordinateurs personnels, utilisez les instructions
dans le dossier [./documentation](../../documentation/mpi.md).

### Compilation et exécution

Pour compiler ce programme, vous pouvez utiliser les commandes suivantes en fonction de vos compilateurs :

Pour g++:
```bash
g++ -O3 main.cpp -o exe
```

Pour Intel (sur Ruche par exemple):
```bash
icpc -O3 main.cpp -o exe
```

La compilation génère un fichier exécutable du nom de `exe`. Vous pouvez lancer le programme en faisant :
```bash
./exe
```

### Arguments en ligne de commande

Il est possible de changer certains paramètres numériques directement en ligne de commande :

```bash
./executable -nx 128 -ny 256 -it 1000
```

- `-nx`: nombre de cellules dans la direction x
- `-ny`: nombre de cellules dans la direction y
- `-it`: nombre d'itérations
- `-d` : fréquence des sorties de fichier en nombre d'itérations
- `-p` : fréquence des affichages dans le terminal en nombre d'itérations

### Visualisation

Le code génère des sorties (fichier sur le disque dur) au format binaire contenant les grilles de champs à un temps donné.
Les fichiers sont toujours écrits dans un dossier `diags`.

Des scripts Python vous sont fournis pour vous aider à visualiser ces données.
Vous pouvez aussi utiliser vos propres scripts dans le langage de votre choix.
Les scripts sont dans le dossier [visualization](./visualization):
- [plot_binary.py](./visualization/plot_binary.py)

```bash
python -i plot_binary.py <chemin vers un fichier diag_*.bin>
```

- [animate.py](./visualization/animate.py)

```bash
python -i animate.py <chemin vers un dossier diags>
```

- [plot_scalar.py](./visualization/plot_scalar.py)

```bash
python -i animate.py <chemin vers un fichier scalars.bin>
```

Voici un exemple d'image produite par la lecture d'un fichier via le script `plot_binary.py`.

<img src="../../support/materiel/maxwell_image.png" height="400">

## Consignes de TP <a id="consignes"></a>

Dans ce TP, vous aurez à modifier un programme séquentiel afin de le rendre parallèle avec le standard MPI.
Vous serez ensuite amené à étudier les performances de la version parallèle.

Le TP se compose de 4 sous-sections :
- [Sous-section I](./consignes/1_sequentiel.md) : il s'agit de la découverte du code séquentiel
- [Sous-section II](./consignes/2_machine.md) : cette partie concerne la découverte de la machine parallèle
- [Sous-section III](./consignes/3_mpi.md) : cette partie est l'implémentation de la version MPI
- [Sous-section IV](./consignes/4_performance.md) : cette partie est l'étude de la performance des codes parallèles
- [Sous-section V](./consignes/5_aide.md) : aide et conseils pour le projet