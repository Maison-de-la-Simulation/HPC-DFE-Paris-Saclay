import os

number_of_cores = [1,2,4,8,16,32,40,64,80]

for cores in number_of_cores:

    folder = "cores_{:02d}".format(cores)
    nodes = 1 + int((cores - 1) / 40)

    print("Case: {} {}".format(cores, nodes))

    os.makedirs(folder)

    os.chdir(folder)

    script = "#!/bin/bash \n"
    script += "#SBATCH --job-name=stng_{} \n".format(cores)
    script += "#SBATCH --output=output \n"
    script += "#SBATCH --error=error \n"
    script += "#SBATCH --ntasks={}                # Nombre d'unité de calcul ou de processus MPI \n".format(cores)
    script += "#SBATCH --nodes={}                # Nombre d'unité de calcul ou de processus MPI \n".format(nodes)
    script += "#SBATCH --time=00:02:00           # Temps souhaité pour la réservation \n"
    script += "#SBATCH --partition=cpu_short     # Partition des jobs rapides \n"
    script += "source ~/env_dfe_hpc.sh \n"
    script += "et -x \n"
    script += "cd ${SLURM_SUBMIT_DIR} \n"
    script += "srun ../exe -it 2000 -nx 5120 -ny 256 -p 100 -d 100 \n"

    fp = open("launch.sh".format(folder), 'w')
    fp.write(script)
    fp.close()

    os.system("sbatch launch.sh".format(folder))

    os.chdir("../")