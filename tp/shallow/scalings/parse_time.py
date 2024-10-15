import sys
import os

path = "./"

runs = os.listdir(path)
runs.sort()

print(runs)

cores  = []
total_times  = []
com_times    = []
reduc_times = []
diag_times   = []

for run in runs:

    local_path = "{}/{}".format(path,run)

    if os.path.isdir(local_path):

        print(" Analyzing {}".format(run))

        number_of_cores = int(run.split("_")[-1])

        cores.append(number_of_cores)

        with open("{}/output".format(local_path)) as f:
            lines = f.readlines()

        total_times.append(float(lines[-4].split("|")[2]))
        com_times.append(float(lines[-3].split("|")[2]))
        reduc_times.append(float(lines[-2].split("|")[2]))
        diag_times.append(float(lines[-1].split("|")[2]))

print("# Nombre de processus")
print("times['cores'] = np.array({})".format(cores))
print("# Temps dans la boucle en temps")
print("times['total'] = np.array({})".format(total_times))
print("# Temps dans les communications point à point")
print("times['com'] = np.array({})".format(com_times))
print("# Temps pour les réductions de l'énergie")
print("times['reduc'] = np.array({})".format(reduc_times))
print("# Temps dans les diags")
print("times['diags'] = np.array({})".format(diag_times))

