# 3. La loi d'Amdahl

La loi d'Amdahl permet de bien comprendre **d'où vient la perte de
scalabilité** d'un code parallèle.

## Modéliser le temps d'exécution

Sur un seul processus, le temps de simulation se décompose en un temps
passé **hors des régions parallèles** et un temps passé **dans les
régions parallèles** :

\[
t(1) = (\alpha_s + \alpha_p)\, t(1), \qquad \alpha_s + \alpha_p = 1
\]

- \(\alpha_s\) : la fraction du temps passée en dehors des régions
  parallèles (temps « séquentiel ») ;
- \(\alpha_p\) : la fraction du temps passée dans les régions parallèles.

Avec \(N\) processus, seule la partie parallèle est divisée par \(N\) :

\[
t(N) = \left(\alpha_s + \frac{\alpha_p}{N}\right) t(1)
\]

--8<-- "performance/amdahl-temps.svg"

!!! info "Que contient \(\alpha_s\) ?"

    La part hors zone parallèle \(\alpha_s\) ne se limite pas au code
    séquentiel (où un seul processus travaille). Elle comprend aussi les
    temps de **synchronisation**, de **communication** et l'attente due
    aux **déséquilibres de charge**.

## L'accélération

On en déduit l'**accélération** (*speed-up*) \(A\) d'un programme
parallèle pour \(N\) processus, en fonction de la fraction séquentielle :

\[
A(N) = \frac{t(1)}{t(N)} = \frac{1}{\alpha_s + \dfrac{1 - \alpha_s}{N}}
\]

Quand \(N\) devient très grand, le terme \((1 - \alpha_s)/N\) tend vers 0
et l'accélération plafonne :

\[
A(N) \xrightarrow[N \to \infty]{} \frac{1}{\alpha_s}
\]

Avec 1 % de temps séquentiel, un code ne peut donc jamais aller plus de
100 fois plus vite, quel que soit le nombre de processus.

## Conséquences sur l'efficacité

L'efficacité en scalabilité forte vaut alors
\(E(N) = A(N) / N\) :

--8<-- "performance/amdahl-efficacite.svg"

| Nombre d'unités \(N\) | \(\alpha_s = 0\) | \(\alpha_s = 0{,}01\) | \(\alpha_s = 0{,}1\) |
|---|---|---|---|
| 10 | 1 | 0,92 | 0,53 |
| 50 | 1 | 0,67 | 0,17 |
| 100 | 1 | 0,50 | 0,09 |

- Pour une fraction séquentielle \(\alpha_s\) donnée, plus le nombre de
  processus est élevé, plus le temps séquentiel devient **dominant** et
  plus le gain apporté par la parallélisation est limité.
- Pour un nombre de processus \(N\) donné, plus la part séquentielle
  \(\alpha_s\) est importante, plus vite la **limite de scalabilité** du
  code est atteinte.

!!! success "À retenir"

    Selon la scalabilité d'un code parallèle, il arrive un moment où
    **augmenter le nombre de processeurs n'accélère plus l'exécution**.
    Réduire la part séquentielle (communications, synchronisations,
    déséquilibres) est la clé pour exploiter un grand nombre d'unités de
    calcul.
