# ______________________________________________________________________________
#
# Exercice 1 : Les tout premiers pas avec MPI
#
# Dans cet exercice, vous apprendrez :
# - à initaliser un programme MPI
# - à récupérer le nombre total de rangs
# - à récupérer le rang pour chaque processus
#
# ______________________________________________________________________________

import mpi4py

# En Python, l'initialisation de MPI est automatique mais ce n'est pas le cas des autres langages comme C, C++ ou Fortran, on désactive donc l'initialisation automatique pour l'exemple :
mpi4py.rc.initialize = False  
mpi4py.rc.finalize = False

# En Python, la bibliothèque MPI se récupère dans le paquet mpi4py
from mpi4py import MPI

# Cette première fonction permet d'initialiser MPI,
# Elle est à appeler avant tout autre action avec MPI.
???

# On récupère le communicateur global
???

# On récupère le nombre total de rang dans le communicateur par défaut
???

# Chaque process récupère son numéro de rang dans le communicateur par défaut
???

# Chaque processus affiche ses informations :
print("Je suis le rang  {} sur {}".format(???, ???))

# On finalise MPI
???

