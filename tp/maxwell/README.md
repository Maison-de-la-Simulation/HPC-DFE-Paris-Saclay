# Simulation des équations de Maxwell via la méthode aux différences finis explicite centrée de Yee

## Présentation du modèle

Nous nous intéressons dans ce projet à la simulation des équations Maxwell par l'utilisation d'une méthode aux différences finies très connue du nom de FDTD pour Finite-Difference Time-Domain publié par K. Yee dans les années 60 et toujours utilisée aujourd'hui dans les codes de calcul pour sa rapidité et sa simplicité.

On rappelle les équations de Maxwell dans le vide :

Maxwell Ampere :

<p align="center"><img src=".extra//fa5e3c8fcfe56486fcae2ea29d7aa026.svg?invert_in_darkmode" align=middle width=219.63344204999999pt height=33.81208709999999pt/></p>

Maxwell Faraday :

<p align="center"><img src=".extra//9a5b774e6c94001adb04d0cea594a90e.svg?invert_in_darkmode" align=middle width=129.8072886pt height=33.81208709999999pt/></p>

Maxwell Gauss :

<p align="center"><img src=".extra//df6d5ac4681bb3f1c44ce35bec7c04d5.svg?invert_in_darkmode" align=middle width=92.1154542pt height=11.232861749999998pt/></p>

Maxwell Flux :

<p align="center"><img src=".extra//6e6f7a4af1ce686a5b47cd64829575d7.svg?invert_in_darkmode" align=middle width=92.3266806pt height=11.232861749999998pt/></p>

Dans le cadre de la discrétisation de Yee, on ne s'intéresse qu'à Maxwell Ampere et Faraday. On commence par normaliser les équations pour éliminer les constantes et réduire l'amplitude des champs (ce qui est toujours préférable en informatique).

<p align="center"><img src=".extra//a148d71e00f346bb4c6c1914b6f18470.svg?invert_in_darkmode" align=middle width=315.70048455pt height=33.81208709999999pt/></p>

En 2D, la discrétisation donne les équations suivantes :

                Ex[iy*nxd+ix] +=
                              + dtdy * (Bz[(iy+1)*nxd + ix] - Bz[iy*nxd+ix]);

<p align="center"><img src=".extra//307263e7baf74cca182e0e5afe981b6d.svg?invert_in_darkmode" align=middle width=339.65512019999994pt height=33.62942055pt/></p>