// ______________________________________________________________________________
//
// Exercice 6 : Création d'une topologie cartésienne
//
// Cet exercice consiste à mettre en place une topologie cartésienne 2D à
// partir des fonctions MPI adéquates.
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
    
    // Creation de la topology cartésienne

    int ranks_per_direction[2] = {3, 4};
    int periodicity[2] = {1, 1};
    int reorganisation = 1;
    MPI_Comm cartesian_communicator;
    

    MPI_Cart_create(MPI_COMM_WORLD,
                         2,
                         ranks_per_direction,
                         periodicity,
                         reorganisation,
                         &cartesian_communicator);
    
    // On récupère le rang dans le nouveau communicateur cartésien
    
    int rank;
    
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    // On récupère les coordonnées du processus courant dans
    // la topologie cartésienne
    
    int rank_coordinates[2];
    
    MPI_Cart_coords( cartesian_communicator, rank, 2, rank_coordinates);

    // On récupère les rangs voisins

    // en -y (rank_neighbors_my), en +y (rank_neighbors_py)
    
    int rank_neighbors_my;
    int rank_neighbors_py;
    
    MPI_Cart_shift( cartesian_communicator, 1, 1, &rank_neighbors_my, &rank_neighbors_py);

    // En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)
    
    int rank_neighbors_mx;
    int rank_neighbors_px;
    
    MPI_Cart_shift( cartesian_communicator, 0, 1, &rank_neighbors_mx, &rank_neighbors_px);
    
    // Chaque processus affiche son rang, ses coordonnées et ses voisins
    // à tour de rôle

    for(int i=0 ; i < number_of_ranks ; i++) {
        if (rank == i) {
            std::cout << " Le rang " << rank << " a pour coordonnées " << rank_coordinates[0] << " " << rank_coordinates[1]
                      << " et pour voisin en -x " << rank_neighbors_mx << ", en + x " << rank_neighbors_px
                      << ", en -y " << rank_neighbors_my << " et en +y " << rank_neighbors_py
                      << std::endl;
        }
        MPI_Barrier(cartesian_communicator);
    }

    
    MPI_Finalize();
}
