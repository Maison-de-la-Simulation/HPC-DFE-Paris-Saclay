// ______________________________________________________________________________
//
// Exercice 3 : communication en anneau
//
// Ce second programme présente le fonctionnement des communications
// bloquantes sendrecv.
// On illustre ici son fonctionnement par une communication en anneau.
// Chaque processus envoie un message à son voisin de rang supérieur.
// et reçoit de son voisin de rang inférieur.
// On effectue un anneau périodique.
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

    if (rank == 0) {
        send_rank = ???;
        recv_rank = ???;
    } else if (rank == number_of_ranks - 1) {
        send_rank = ???;
        recv_rank = ???;
    } else {
        send_rank = ???;
        recv_rank = ???
    }

    std::cout << "Le rang " << rank << " envoie le message " << rank << " au rang " << send_rank << std::endl;

    ierror = MPI_Barrier(MPI_COMM_WORLD);

    ierror = MPI_???(???,                       // La partie du tableau à envoyer
                  ???,                      // Le nombre d'éléments
                  ???,                      // Le type de donnée utilisé
                  ???,                      // Le rang du voisin destinataire
                  tag,                      // tag de la communication
                  ???,               // La partie du tableau qui va recevoir les données
                  ???,               // Le nombre d'éléments
                  ???,               // Le type de donnée pour les données reçues
                  ???,               // Le rang du voisin qui va nous envoyer des données
                  tag,                        // tag de la communication
                  MPI_COMM_WORLD,             // Le communicateur
                  MPI_STATUS_IGNORE          // Status de la communication
                );

    // On affiche les résultats

    std::cout << "Le rang " << rank << " a reçu le message " << recv_message << " du rang " << recv_rank << "." << std::endl;

    // Finalisation de MPI

    MPI_Finalize();

}
