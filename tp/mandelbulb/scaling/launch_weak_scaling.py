# _________________________________________________________________
#
# This script is used to launch a weak scaling study 
# of the Mandelbulb
# __________________________________________________________________

import os

cells_per_dir = 128
ranks_per_dir = [ [1,1,1], [1,1,2], [1,2,2], [2,2,2], [2,2,4], [2,4,4], [4,4,4] ]

for config in ranks_per_dir:

    cores = config[0] * config[1] * config[2]
    cells = [cells_per_dir * config[0], cells_per_dir * config[1], cells_per_dir * config[2]]

    folder = "cores_{:02d}".format(cores)
    nodes = 1 + int((cores - 1) / 40)

    print("Case: {} {} (cells {})".format(cores, nodes, cells))

    os.makedirs(folder)

    os.chdir(folder)

    script = "#!/bin/bash \n"
    script += "#SBATCH --job-name=weak_{} \n".format(cores)
    script += "#SBATCH --output=output \n"
    script += "#SBATCH --error=error \n"
    script += "#SBATCH --ntasks={}                # Nombre d'unite de calcul ou de processus MPI \n".format(cores)
    script += "#SBATCH --nodes={}                # Nombre d'unite de calcul ou de processus MPI \n".format(nodes)
    script += "#SBATCH --time=00:10:00           # Temps souhaite pour la reservation \n"
    script += "#SBATCH --partition=cpu_short     # Partition des jobs rapides \n"
    script += "#SBATCH --exclusive\n"
    script += "set -x \n"
    script += "cd ${SLURM_SUBMIT_DIR} \n"

    script += "ulimit -a \n"
    script += "source /gpfs/workdir/labotm/Installations/miniforges3/m2dfe_env.sh \n"
    script += "conda activate m2dfe \n"

    script += "mpirun -np {} python3 ../../mandelbulb.py --domain_size {} {} {} --ranks_per_dir {} {} {} \n".format(cores,cells[0], cells[1], cells[2], config[0], config[1], config[2])

    fp = open("launch.sh".format(folder), 'w')
    fp.write(script)
    fp.close()

    os.system("sbatch launch.sh".format(folder))

    os.chdir("../")
