// ______________________________________________________________________________
//
// Exercice 4 : Communication non bloquante point à point
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
    // Initialisation de MPI

    MPI_Init( &argc, &argv );

    // Nombre total de rangs

    int number_of_ranks;

    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );

    // Rang du processus MPI

    int rank;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    // Tous les processus ont la variable message initialisée à 0

    int tag = 0;
    int send_rank;
    int recv_rank;
    int recv_message;
    int ierror;
    MPI_Request request[2];

    //MPI_Request * request = new MPI_Request[2];
    //delete [] request;

    if (rank == 0) {
        send_rank = 1;
        recv_rank = number_of_ranks - 1;
    } else if (rank == number_of_ranks - 1) {
        send_rank = 0;
        recv_rank = rank - 1;
    } else {
        send_rank = rank + 1;
        recv_rank = rank - 1;
    }

    std::cout << "Le rang " << rank << " envoie le message " << rank << " au rang " << send_rank << std::endl;

    ierror =  MPI_Barrier(MPI_COMM_WORLD);

    MPI_Irecv(&recv_message, 1, MPI_INT, recv_rank, tag, MPI_COMM_WORLD, &request[0]);
    MPI_Isend(&rank, 1, MPI_INT, send_rank, tag, MPI_COMM_WORLD, &request[1]);

    // ici je suis non bloquant

    MPI_Waitall(2,request,MPI_STATUS_IGNORE);

    // On affiche les résultats

    std::cout << "Le rang " << rank << " a reçu le message " << recv_message << " du rang " << recv_rank << "." << std::endl;

    // Finalisation de MPI

    MPI_Finalize();

}
