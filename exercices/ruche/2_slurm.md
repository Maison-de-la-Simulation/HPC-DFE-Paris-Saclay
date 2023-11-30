# Exercice: lancement d'un job avec Slurm sur Ruche

Avant de commencer cet exercice, vous devez être connecté au super-calculateur Ruche.

Dans cet exercice nous allons lancer un *job* SLURM sur un noeud de calcul.
Nous allons utiliser un script de lancement pour cela.

Le but de cet exercice est de vous familiariser avec le lancement de jobs sur Ruche et de récupérer des informations sur la machine.

1) Rendez-vous dans votre espace de travail $WORK qui est plus approprié pour ce cours :

```bash
cd $WORK
```

2) Créer un dossier `test_slurm` et rendez-vous dedans :

```bash
mkdir test_slurm
cd test_slurm
```

1) **Création d'un script de lancement**: dans ce dossier, créez un script `launch.slurm` avec le contenu suivant :

```bash
#!/bin/bash
#SBATCH --job-name=master_dfe     # Nom du job
#SBATCH --output=output           # fichier qui réceptionne la sortie standard
#SBATCH --error=error             # fichier qui réceptionne la sortie d'erreur
#SBATCH --ntasks=1                # Nombre d'unité de calcul ou de processus MPI
#SBATCH --time=00:05:00           # Temps souhaité pour la réservation
#SBATCH --partition=cpu_short     # Partition des jobs rapides

# Permet de s'assurer que l'exécutiona lieu dans le dossier de soumission du job
cd $SLURM_SUBMIT_DIR

set -x

# On regarde les propriétés des processeurs via la commande lscpu
lscpu

# Attente de 10 secondes pour faire durer un peu le job
sleep 10
```

2) **Lancement du job**: lancez le job en faisant :

```bash
sbatch launch.slurm
```

3) **Vérification du job**: vérifiez que le job est bien en cours d'exécution en faisant :

```bash
squeue -u <login>
```

Vous devrier voir le job en attente ou en cours d'exécution.

4) **Résultats du job**: une fois le job terminé, vous devriez voir deux fichiers `output` et `error` qui ont été créés. Regardez leur contenu en faisant :

```bash
cat output
cat error
```

Si tout s'est bien passé, la sortie standard se trouve dans le fichier `output` avec la sortie de la commande `lscpu`
et la sortie d'erreur dans le fichier `error` avec rien dedans.

Vous devriez avoir quelque chose comme ça : 
    
```bash
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                40
On-line CPU(s) list:   0-39
Thread(s) per core:    1
Core(s) per socket:    20
Socket(s):             2
NUMA node(s):          2
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 85
Model name:            Intel(R) Xeon(R) Gold 6230 CPU @ 2.10GHz
Stepping:              7
CPU MHz:               2100.000
CPU max MHz:           2100,0000
CPU min MHz:           800,0000
BogoMIPS:              4200.00
Virtualization:        VT-x
L1d cache:             32K
L1i cache:             32K
L2 cache:              1024K
L3 cache:              28160K
NUMA node0 CPU(s):     0-19
NUMA node1 CPU(s):     20-39
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc aperfmperf eagerfpu pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch epb cat_l3 cdp_l3 invpcid_single intel_ppin intel_pt ssbd mba ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 hle avx2 smep bmi2 erms invpcid rtm cqm mpx rdt_a avx512f avx512dq rdseed adx smap clflushopt clwb avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local dtherm arat pln pts pku ospke avx512_vnni md_clear spec_ctrl intel_stibp flush_l1d arch_capabilities
```

En cas de problème, c'est souvent la sortie erreur qui reçoit les messages d'erreur.

5) Afin de connaître maintenant la quantité de RAM disponible, nous allons utiliser la commande `free -h`. Reprenez le script `launch.slurm` et remplacez la commande lscpu par la commande `free -h`. Lancez le job.

Vous devriez obtenir quelque chose comme ça :

```bash
              total        used        free      shared  buff/cache   available
Mem:           188G         10G        175G        2,1G        2,2G        175G
Swap:            0B          0B          0B
```








