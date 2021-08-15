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
    
    MPI_Init( &argc, &argv );
    
    // Cette subroutine permet de récupérer le nombre total de rangs dans
    // le communicateur par défaut MPI_COMM_WORLD.
    
    int number_of_ranks;
    
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );
    
    // Cette subroutine permet de récupérer le numéro du rang du processus
    // qui appelle cette subroutine dans
    // le communicateur par défaut MPI_COMM_WORLD.
    
    int rank;
    
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    int N = 4000000;
    int local_N = N / number_of_ranks;
    double time;
    
    // std::vector<double > A(local_N);
    // std::vector<double > B(local_N);
    // std::vector<double > C(local_N);
    
    double * A = new double[local_N];
    double * B = new double[local_N];
    double * C = new double[local_N];
    
    // Initialization
    
    for(int i = 0 ; i < local_N ; i++) {
        A[i] = sin(4.0*i);
        B[i] = pow(sin(10.0*i),2);
        C[i] = cos(5.0*i);
    }

    // Boucle de calcul
    
    time = MPI_Wtime();

    for(int i = 1 ; i < local_N-1 ; i++) {
        A[i] = B[i] - C[i] + (B[i-1] + B[i+1] + 4*B[i]);
    }

    time = MPI_Wtime() - time;
    
    // Chaque rang affiche son numéro/identifiant (variable rank)

    std::cout << "Je suis le rang " << rank << " / " << number_of_ranks-1 << " avec un temps de " << time << " s" << std::endl;
    
    delete [] A;
    delete [] B;
    delete [] C;
    
    MPI_Finalize();
}
