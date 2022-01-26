// ______________________________________________________________________________
//
// Exercice : Utilisation du type de donnée dérivé vector
//
// Cet exercice permet de tester l'utilisation du type dérivé vector.
// Chaque processus initialise un tableau 2D et envoie sa colonne
// la plus à droite au processeur suivant qui la réceptionne dans sa
// colonne la plus à gauche.
//
//
// Documentation supplémentaire :
// - http://www.idris.fr/media/formations/mpi/idrismpi.pdf
// - https://www.rookiehpc.com/mpi/docs/mpi_type_vector.php
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

    // On récupère le rang

    int rank;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    // Création du tableau local à chaque rang
    // On initialise avec le numéro du rang

    int tag = 0;

    int nx = 10;
    int ny = 16;

    int array[ny][nx];

    for (int iy = 0 ; iy < ny ; iy++) {
        for (int ix = 0 ; ix < nx ; ix++) {
             array[iy][ix] = rank;
        }
    }

    // Creation du type dérivé
    MPI_Datatype column;
    MPI_Type_vector(????);
    MPI_Type_commit(????);

    // Gestion des destinataires (anneau)

    int send_rank;
    int recv_rank;

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

    // Envoie des éléments via des communications non bloquantes

    MPI_Request request[2];

    MPI_Irecv(???, ???, ???, ???, tag, MPI_COMM_WORLD, ???);

    MPI_Isend(???, ???, ???, ???, tag, MPI_COMM_WORLD, ???);

    MPI_Waitall(???,???,MPI_STATUS_IGNORE);

    // Affichage des colonnes reçues

    for(int irank = 0 ; irank < number_of_ranks ; irank++) {
        if (rank == irank) {
            std::cout << " Rank: " << irank << " - " ;
            for (int iy = 0 ; iy < ny ; iy++) {
                std::cout << array[iy][0] << " ";
            }
            std::cout << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
