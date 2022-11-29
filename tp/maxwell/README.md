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

                Ex[iy*nxd+ix] +=
                              + dtdy * (Bz[(iy+1)*nxd + ix] - Bz[iy*nxd+ix]);

<p align="center"><img src=".extra//307263e7baf74cca182e0e5afe981b6d.svg?invert_in_darkmode" align=middle width=339.65512019999994pt height=33.62942055pt/></p>