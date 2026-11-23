# V. Mesure de la performance

Écrire un programme parallèle ne suffit pas : il faut **mesurer** ce que
la parallélisation apporte vraiment. Ce chapitre présente les notions et
les outils pour évaluer la performance d'un code parallèle et comprendre
d'où viennent ses limites.

## Objectifs

À la fin de ce chapitre, vous saurez :

- identifier un **déséquilibre de charge** et ce qu'il coûte ;
- mener une étude de **scalabilité faible** et de **scalabilité forte**,
  et tracer les courbes d'efficacité correspondantes ;
- expliquer, avec la **loi d'Amdahl**, pourquoi ajouter des unités de
  calcul finit par ne plus accélérer un programme.

## Plan du chapitre

<div class="grid cards" markdown>

-   __1. Déséquilibre de charge et parallélisme parfait__

    ---

    Le temps d'attente, une ressource gaspillée ; le cas idéal du
    parallélisme parfait.

    [:octicons-arrow-right-24: Lire](notions.md)

-   __2. Scalabilité__

    ---

    Mesurer le temps, scalabilité faible et forte, efficacité,
    hyper-scalabilité. **Exercice.**

    [:octicons-arrow-right-24: Lire](scalabilite.md)

-   __3. La loi d'Amdahl__

    ---

    Modéliser le temps d'exécution et comprendre la perte de scalabilité.

    [:octicons-arrow-right-24: Lire](amdahl.md)

</div>
