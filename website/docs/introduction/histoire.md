# 2. Un peu d'histoire

Pour mesurer le chemin parcouru, certaines machines sont comparées à un appareil
que vous connaissez bien : votre smartphone.

## 1940 – 1960 : les prémices

<div class="grid" markdown>

<div markdown>

L'histoire des ordinateurs démarre doucement un peu avant la Seconde Guerre
mondiale, et voit ses premiers systèmes appliqués (électromécaniques ou
électroniques) pendant la guerre.

**ENIAC** (1945), l'un des premiers ordinateurs électroniques.

<span class="timeline-meta"><span>🇺🇸 USA</span><span>50 kflop/s</span><span>150 kW</span></span>

</div>

![ENIAC](../assets/images/introduction/eniac.jpg)

</div>

## 1960 – 1970 : la naissance d'une industrie

<div class="grid" markdown>

<div markdown>

Les super-ordinateurs destinés au calcul scientifique au sens propre
émergent au début des années 60. C'est aussi la naissance du **Fortran**.

**CDC 6600** (1964), l'un des premiers vrais succès du HPC.

<span class="timeline-meta"><span>🇺🇸 USA</span><span>0,5 Mflop/s</span><span>30 kW</span></span>

</div>

![CDC 6600](../assets/images/introduction/cdc6600.jpg)

</div>

## 1970 – 1990 : l'ère des machines vectorielles et de Cray

Seymour Cray, l'un des concepteurs du CDC 6600, fonde sa propre société,
qui devient l'un des leaders du marché pendant 15 ans.

Les Cray utilisent un **processeur vectoriel** : le même calcul est
effectué en même temps sur tous les éléments d'un vecteur de données.

<div class="grid cards" markdown>

-   ![Cray-1](../assets/images/introduction/cray1.jpg)

    **Cray-1** (1975) — vectorisation très efficace, à la fois esthétique
    et performant.

    <span class="timeline-meta"><span>🇺🇸 USA</span><span>160 Mflop/s</span><span>115 kW</span><span>📱 ≈ iPhone 1</span></span>

-   ![Cray Y-MP](../assets/images/introduction/cray-ymp.jpg)

    **Cray Y-MP** (1988) — 2 à 8 processeurs vectoriels à mémoire
    partagée, 333 Mflop/s par processeur.

    <span class="timeline-meta"><span>🇺🇸 USA</span><span>2,6 Gflop/s</span><span>📱 ≈ iPhone 5</span></span>

</div>

## 1990 – aujourd'hui : le parallélisme massif distribué

D'un point de vue économique et technique, il devient de plus en plus
intéressant de **mettre en réseau des processeurs issus du grand public**.
On utilise des processeurs de la micro-informatique légèrement adaptés au
monde du HPC, ce qui permet :

- l'homogénéisation des outils de développement ;
- l'abaissement des coûts de R&D et de production ;
- le retour au processeur **scalaire** (une opération sur une donnée).

<div class="grid cards" markdown>

-   ![CM-5](../assets/images/introduction/cm5.jpg)

    **CM-5** (1993) — 1024 processeurs, premier du TOP500 en 1993.

    <span class="timeline-meta"><span>🇺🇸 USA</span><span>131 Gflop/s</span><span>📱 ≈ iPhone 7</span></span>

-   ![Cluster de PlayStation 3](../assets/images/introduction/cluster-ps3.jpg)

    Exemple surprenant : de véritables **clusters de PlayStation 3** ont été
    construits entre 2007 et 2010, grâce au processeur Cell d'IBM.

</div>

## 2010 – aujourd'hui : le retour de la vectorisation et du multi-cœur

Les processeurs deviennent **multi-cœurs** et redeviennent **vectoriels**,
comme l'étaient les machines Cray. Quelques exemples de processeurs
modernes utilisés en HPC :

<div class="grid cards" markdown>

-   ![Intel Xeon Phi](../assets/images/introduction/xeon-phi.jpg)

    **Intel Xeon Phi KNL** (2016)

    <span class="timeline-meta"><span>72 cœurs</span><span>3 Tflop/s</span><span>200 W</span></span>

-   ![AMD Epyc](../assets/images/introduction/amd-epyc.jpg)

    **AMD Epyc Genoa** (2022)

    <span class="timeline-meta"><span>96 cœurs</span><span>8 Tflop/s</span><span>400 W</span></span>

</div>

## 2010 – aujourd'hui : l'émergence des accélérateurs GPU

La technologie **GPU** offre, pour du calcul pur, de meilleurs ratios
performance / prix / consommation électrique que les CPU. Beaucoup des
super-calculateurs les plus puissants du monde l'utilisent.

!!! info "Accélérateur"

    Les GPU sont des **accélérateurs** : ils complètent le CPU, ils ne le
    remplacent pas.

<div class="grid cards" markdown>

-   ![NVIDIA A100](../assets/images/introduction/nvidia-a100.jpg){ .on-white }

    **NVIDIA A100**

    <span class="timeline-meta"><span>10 Tflop/s</span><span>250 W</span></span>

-   ![AMD MI250](../assets/images/introduction/amd-mi250.jpg){ .on-white }

    **AMD MI250**

    <span class="timeline-meta"><span>45 Tflop/s</span><span>500 W</span></span>

</div>

## La course à l'exascale

Une compétition mondiale vise à atteindre la puissance de \(10^{18}\)
opérations par seconde en double précision (**1 Eflop/s**) dans une
enveloppe énergétique de 20 MW.

<div class="grid" markdown>

<div markdown>

- Ce seuil a été franchi pour la première fois par les États-Unis en 2022
  avec **Frontier**.
- L'Europe se dote à son tour de machines exascale dans le cadre de
  l'initiative EuroHPC.
  <!-- TODO : actualiser (JUPITER en Allemagne, Alice Recoque en France) -->

</div>

<figure markdown>
![Évolution des performances du TOP500](../assets/images/introduction/top500-evolution.png){ .on-white }
<figcaption>Évolution de la puissance mesurée dans le TOP500 (moyenne, somme et maximum), en Gflop/s, échelle logarithmique.</figcaption>
</figure>

</div>

<figure markdown>
![Le super-calculateur Frontier](../assets/images/introduction/frontier.jpg)
<figcaption><strong>Frontier</strong> (USA), premier super-ordinateur à atteindre une puissance exaflopique : \(10^{18}\) opérations par seconde en double précision.</figcaption>
</figure>

## Testez-vous

??? question "Qu'est-ce qu'un processeur vectoriel ?"

    Un processeur qui applique la **même opération** à tous les éléments
    d'un vecteur de données **en même temps**, au lieu de traiter un
    élément après l'autre (processeur scalaire).

??? question "Pourquoi les super-calculateurs utilisent-ils depuis les années 90 des processeurs issus du grand public ?"

    Pour des raisons économiques et techniques : coûts de R&D et de
    production réduits, et outils de développement communs avec le reste
    de l'informatique.
