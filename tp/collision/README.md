# Simulation de particules par la mécanique du point avec collisions

Ce cas applicatif simule un ensemble de particules (sphère) par les équations classiques de Newton en 3D.
La simulation intègre l'effet des frottements et de la gravité.
Elle intègre également un module de collisions.

L'équation du mouvement qui s'applique à chaque particule est la suivante :
<p align="center"><img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//3ade19f5b4a6f3d812e3b551a00193ce.svg?invert_in_darkmode" align=middle width=137.90964pt height=38.250465pt/></p>
Où <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//8b5bd5fd95868f24ad0a078d34768d7d.svg?invert_in_darkmode" align=middle width=10.747770000000003pt height=23.488740000000007pt/> est le vecteur gravité, <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode" align=middle width=14.433210000000003pt height=14.155350000000013pt/> la masse de la particule, <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//c745b9b57c145ec5577b82542b2df546.svg?invert_in_darkmode" align=middle width=10.576500000000003pt height=14.155350000000013pt/> un coefficient de freinage
et <img src="https://rawgit.com/in	git@github.com:Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/year-2020-2021/.extra//cd74c822d31d457e590f28706c11499d.svg?invert_in_darkmode" align=middle width=10.747770000000004pt height=23.488740000000007pt/> le vecteur vitesse de la particule.
