# Simulation des équations de Maxwell via la méthode aux différences finis explicite centrée de Yee

## Présentation du modèle

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

En 2D, la discrétisation donne les équations suivantes :
        
        // Ez (primal, primal)
        for (int iy = 0 ; iy < nyp ; iy++) {
            for (int ix = 0 ; ix < nxp ; ix++) {
                Ez[iy*nxp+ix] +=
                                + dtdx * (By[iy*nxd+ix+1] - By[iy*nxd+ix])
                                - dtdy * (Bx[(iy+1)*nxp+ix] - Bx[iy*nxp+ix]);

<p align="center"><img src=".extra//ad7015d0a9a6cf54e178878906df03c7.svg?invert_in_darkmode" align=middle width=318.42411315pt height=36.82577085pt/></p>

<p align="center"><img src=".extra//c8e58c2c82b57b667d40e0b52121fa3b.svg?invert_in_darkmode" align=middle width=317.08854045pt height=33.62942055pt/></p>

<p align="center"><img src=".extra//11eb95a1f61dfd6015539c701060ebef.svg?invert_in_darkmode" align=middle width=529.30628685pt height=36.82577085pt/></p>

Où <img src=".extra//c461456e376ee5b83866a0c882334b0f.svg?invert_in_darkmode" align=middle width=65.57458379999998pt height=24.65753399999998pt/> est l'iteration en temps, <img src=".extra//77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> et <img src=".extra//36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/> respectivement la discrétisation en espace suivant les directions x et y.

<img src="../../support/materiel/maxwell_yee_grid.png" height="400">
