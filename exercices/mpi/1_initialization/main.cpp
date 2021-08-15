// ______________________________________________________________________________
//
// Exercice 1 : Les tout premiers pas avec MPI
//
// Dans cet exercice, vous apprendrez :
// - à initaliser un programme MPI
// - à récupérer le nombre total de rangs
// - à récupérer le rang pour chaque processus
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

// il est important d'inclure la bibliothèque
// sans quoi aucun de vos appels ne sera reconnu.
#include <mpi.h>

int main( int argc, char *argv[] )
{
    
    // Cette première fonction permet d'initialiser MPI,
    // elle est à appeler avant tout autre action avec MPI.
    
    MPI_???( &argc, &argv );
    
    // Cette subroutine permet de récupérer le nombre total de rangs dans
    // le communicateur par défaut MPI_COMM_WORLD.
    
    int number_of_ranks;
    
    MPI_???( ???, &??? );
    
    // Cette subroutine permet de récupérer le numéro du rang du processus
    // qui appelle cette subroutine dans
    // le communicateur par défaut MPI_COMM_WORLD.
    
    int rank;
    
    MPI_???( ???, &??? );
    
    // Chaque rang affiche son numéro/identifiant (variable rank)

    std::cout << "Je suis le rang " << rank << " / " << number_of_ranks-1 << std::endl;
    
    MPI_???();
}
