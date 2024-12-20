## IV. Etude de performance

Dans cette dernière partie, nous allons maintenant étudier la performance du code parallèle.
Pour cela nous allons regarder les deux types de passage à l'échelle que sont le *weak* ou *strong scaling*.
Dans chaque cas, vous serez amené à faire d'abord votre propre étude puis à étudier une étude faite sur le super-calculateur Ruche.

Pour rappel, sur Ruche, vous pouvez charger soit l'environnement Intel :
```bash
module purge
module load anaconda3/2022.10/gcc-11.2.0
module load intel/20.0.4/gcc-4.8.5
module load intel-mpi/2019.9.304/intel-20.0.4.304
export I_MPI_CXX=icpc
```

Ou GNU :
```bash
module purge
module load anaconda3/2022.10/gcc-11.2.0
module load gcc/11.2.0/gcc-4.8.5
module load openmpi/4.1.1/gcc-11.2.0
```

Vous pouvez compiler le code avec la commande suivante pour Intel : 

```bash
mpiicpc -O3 main.cpp -o exe
```

Ou pour GNU :

```bash
mpicxx -O3 main.cpp -o exe
```


Le *wrapper* `mpiicpc` est celui utilisé par Intel.
Attention à ne pas faire appel à `mpicxx` qui utilise `g++` dans ce cas.

### 4.1. Weak scaling

Regardons dans un premier temps le passage à l'échelle faible ou *weak scaling*.
Pour rappel, ce type d'étude consiste à regarder comment évolue l'efficacité du
code lorsque l'on augmente le nombre de processus tout en gardant la
charge constante par unité de calcul.

a) Effectuez un test de weak scaling jusqu'à 80 coeurs (2 noeuds) en choisissant les paramètres suivants pour le cas utilisant 1 processus :
- `nx = 512`
- `ny = 512`
- 2000 itérations

Si les temps fluctuent beaucoup, il peut être nécessaire de faire une moyenne sur plusieurs simulations voir d'éliminer les points abérrants que l'on rencontre parfois sur Ruche (communications excessivement longues par rapport à la moyenne)

b) Calculez quel est le coût mémoire théorique d'un domaine MPI en ne considérant que le poids des grilles (et pas toutes les petites variables)

c) Affichez la courbe en temps et d'efficacité correspondante en y faisant apparaître le temps dans chaque partie :
- calcul
- communications point à point
- communications globales 

Nous avons de notre côté également effectué ce test :

<img src="../../../support/materiel/maxwell_mpi_weak_scaling_time.png" height="400">
<img src="../../../support/materiel/maxwell_mpi_weak_scaling_efficiency.png" height="400">
<img src="../../../support/materiel/maxwell_mpi_weak_scaling_part.png" height="400">


d) Commentez ce *weak scaling* en vous aidant de toutes les courbes

### 4.2. Strong scaling

Regardons maintenant le passage à l'échelle fort ou *strong scaling*.
Pour rappel, ce type d'étude consiste à regarder comment évolue l'efficacité du
code lorsque l'on augmente le nombre de processus tout en gardant la
charge globale constante.

a) Effectuez un test de *strong scaling* jusqu'à 80 coeurs (2 noeuds) en choisissant les paramètres suivants :
- `nx = 5120`
- `ny = 256`
- 2000 itérations

b) Affichez la courbe en temps et d'efficacité correspondante en y faisant apparaître le temps dans chaque partie :
- calcul
- communications point à point
- communications globales

Nous avons de notre côté également effectué ce test :

<img src="../../../support/materiel/maxwell_mpi_strong_scaling_time.png" height="450">
<img src="../../../support/materiel/maxwell_mpi_strong_scaling_efficiency.png" height="400">
<img src="../../../support/materiel/maxwell_mpi_strong_scaling_part.png" height="400">

c) Commentez ce *strong scaling* en vous aidant de toutes les courbes.