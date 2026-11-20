# 2. Scalabilité

## Définition

Scalabilité, ou passage à l'échelle (*scalability*)
:   La capacité d'un programme parallèle à tirer parti d'un nombre
    croissant d'unités de calcul sur un système donné.

- L'étude de scalabilité est **systématique** pour déterminer le potentiel
  d'un code parallèle.
- Elle se présente sous la forme d'un **graphe**, en fonction du nombre
  d'unités de calcul.
- Elle permet de déterminer le **nombre maximal d'unités de calcul
  utiles** pour résoudre un problème.

On distingue deux études : la scalabilité **faible** et la scalabilité
**forte**. Dans les deux cas, un processus tourne sur une seule unité de
calcul.

## Mesurer le temps

Une étude de scalabilité repose sur des mesures de temps. Avec MPI, la
fonction `MPI.Wtime()` renvoie le temps écoulé en secondes sur le
processus courant. Deux appels et une soustraction donnent le temps passé
dans une section du code :

=== "Python"

    ```python
    start = MPI.Wtime()

    # Des calculs...

    elapsed = MPI.Wtime() - start
    ```

=== "C / C++"

    ```cpp
    double start = MPI_Wtime();

    // Des calculs...

    double elapsed = MPI_Wtime() - start;
    ```

:material-book-open-variant: [Documentation de `MPI_Wtime` (RookieHPC)](https://rookiehpc.github.io/mpi/docs/mpi_wtime/index.html)

Chaque processus mesure son propre temps. Pour une étude de scalabilité,
on retient en général le temps du processus **le plus lent**, qui
détermine la durée de l'exécution.

## Scalabilité faible

Scalabilité faible (*weak scalability*)
:   On garde la **charge constante par processus** tout en augmentant le
    nombre de processus.

La taille totale du domaine augmente donc proportionnellement au nombre
d'unités de calcul. Le nombre de communications et de synchronisations
augmente lui aussi de la même manière.

--8<-- "performance/scalabilite-faible.svg"

On trace en général l'**efficacité de scalabilité faible** :

\[
E_{\text{faible}}(N) = \frac{t(1)}{t(N)}
\]

où \(t(1)\) est le temps de simulation pour un processus et \(t(N)\) le
temps de simulation pour \(N\) processus. Dans le cas parfait, chaque
processus a toujours la même charge : le temps ne change pas et
l'efficacité vaut 1.

## Scalabilité forte

Scalabilité forte (*strong scalability*)
:   On garde la **charge totale constante** tout en augmentant le nombre de
    processus. La charge par processus diminue donc quand le nombre de
    processus augmente.

Le domaine garde sa taille, mais il est découpé en sous-domaines de plus en
plus petits : le nombre de communications et de synchronisations augmente
avec le nombre de subdivisions.

--8<-- "performance/scalabilite-forte.svg"

On trace en général l'**efficacité de scalabilité forte** :

\[
E_{\text{forte}}(N) = \frac{t(1)}{N \times t(N)}
\]

Dans le cas parfait, \(N\) processus vont \(N\) fois plus vite :
\(t(N) = t(1) / N\) et l'efficacité vaut 1. Le rapport
\(A(N) = t(1) / t(N)\) est l'**accélération** (*speed-up*), et
\(E_{\text{forte}}(N) = A(N) / N\).

## Allure des courbes d'efficacité

--8<-- "performance/efficacite.svg"

- **Parallélisme parfait** : l'efficacité reste à 1 quel que soit le
  nombre d'unités de calcul.
- **Scalabilité réelle** : l'efficacité diminue quand le nombre d'unités
  augmente, à cause des communications, des synchronisations et des
  déséquilibres de charge.
- **Hyper-scalabilité** : l'efficacité dépasse 1. Cela arrive par exemple
  en scalabilité forte quand, une fois découpées, les données de chaque
  processus tiennent dans les mémoires cache du processeur, beaucoup plus
  rapides que la RAM.

!!! example "Exercice — Étude de passage à l'échelle"

    Mesurer les temps de calcul du code d'intégration numérique de
    l'exercice 8, puis réaliser une étude de scalabilité faible et forte
    sur Ruche.

    :material-file-document: [Énoncé de l'exercice](https://github.com/Maison-de-la-Simulation/HPC-DFE-Paris-Saclay/tree/master/exercices/performance)

    Dans votre éditeur : `exercices/performance/README.md`.
