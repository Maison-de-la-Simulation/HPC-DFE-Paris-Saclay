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
#include <iomanip>

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
    int topology_map[ranks_per_direction[0]*ranks_per_direction[1]];
    

    MPI_Cart_create(MPI_COMM_WORLD,
                         2,
                         ranks_per_direction,
                         periodicity,
                         reorganisation,
                         &cartesian_communicator);
    
    // On récupère le rang dans le nouveau communicateur cartésien
    
    int rank;
    
    MPI_Comm_rank( cartesian_communicator, &rank );
    
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

    // Communication de la topologie totale au rang 0
    
    MPI_Gather(&rank,1,MPI_INT,&topology_map,1,MPI_INT,0,cartesian_communicator);
    
    // Affichage de la topologie
    
    if (rank == 0) {
    
        std::cout << std::endl;
        std::cout <<  " Carte de la topologie : "<< std::endl;
        std::cout <<  " ---------------------------> y"<< std::endl;
    
        for(int iy = 0; iy < ranks_per_direction[0] ; iy++) {
            for(int ix = 0; ix < ranks_per_direction[1] ; ix++) {
                
                std::cout << " | " << std::setw(3) << topology_map[iy*ranks_per_direction[1] + ix] ;
                
            }
            std::cout << std::endl;
        }
    
        std::cout << " v" << std::endl;
        std::cout << " x" << std::endl;
    
    }
    
    // Construction de la carte de la topologie  à partir de MPI_Cart_coords :
    
    int coordinates[2];
    
    for (int i = 0 ; i < number_of_ranks ; i++) {
        MPI_Cart_coords( cartesian_communicator, i, 2, coordinates);
        topology_map[coordinates[0]*ranks_per_direction[1] + coordinates[1]] = i;
    }

    // Affichage de la topologie
    
    if (rank == 0) {
    
        std::cout << std::endl;
        std::cout <<  " Carte de la topologie : "<< std::endl;
        std::cout <<  " ---------------------------> y"<< std::endl;
    
        for(int iy = 0; iy < ranks_per_direction[0] ; iy++) {
            for(int ix = 0; ix < ranks_per_direction[1] ; ix++) {
                
                std::cout << " | " << std::setw(3) << topology_map[iy*ranks_per_direction[1] + ix] ;
                
            }
            std::cout << std::endl;
        }
    
        std::cout << " v" << std::endl;
        std::cout << " x" << std::endl;
    
    }
    
    MPI_Finalize();
}
