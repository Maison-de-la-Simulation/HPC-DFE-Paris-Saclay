// ______________________________________________________________________________
//
// Exercice 2 : communication bloquante point à point
//
// Ce second programme présente le fonctionnement des communications
// bloquantes send/recv
// Dans ce code, le processus MPI de rang 1 envoie un message
// au processus de rang 2.
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
    
    // Initialisation de MPI
    
    MPI_Init( &argc, &argv );
    
    // Nombre total de rangs
    
    int number_of_ranks;
    
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );
    
    // Rang du processus MPI
    
    int rank;
    
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    // Tous les processus ont la variable message initialisée à 0

    int message;
    int tag = 0;

    // Envoi du message du rang 1 vers le rang 2

    if (rank == 1) {
        
        message = 42;

        std::cout << "Le rang " << rank 
                  << " envoie le message " << message 
                  << " au rang 2 / " << number_of_ranks << "." 
                  << std::endl;

        MPI_Send(&message,                   // La partie du tableau à envoyer
                      1,                         // Le nombre d'élèments
                      MPI_INTEGER,               // Le type de donnée utilisé
                      2,                         // Le rang du destinataire
                      tag,                       // tag de la communication
                      MPI_COMM_WORLD);            // Le communicateur
        
    }
    
    // Reception du message du rang 2 venant du rang 1
    
    if (rank == 2) {
        
        MPI_Recv(&message,                   // La partie du tableau à envoyer
                      1,                         // Le nombre d'élèments
                      MPI_INTEGER,               // Le type de donnée utilisé
                      1,                         // Le rang de l'expéditeur
                      tag,                       // tag de la communication
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE);            // Le communicateur
        
                      
        std::cout << "Le rang " << rank 
                  << " a reçu le message " << message 
                  << " du rang 1 / " << number_of_ranks << "." 
                  << std::endl;
        
    }
    
    MPI_Finalize();
}
