// ______________________________________________________________________________
//
// Exercice 5 : Utilisation des communications collectives MPI_REDUCE
//
//
// Dans cet exercice, nous intégrons la fonction f(x) = sin(x) de
// manière numérique entre 0 et pi/2 et de manière parallèle
// On utilis une intégration par la méthode des rectangles.
// Le domaine est subdivisé suivant le nombre de processus MPI demandé.
//
//
//          delta
// 0         <>                                          pi/2
// |                 |                 |                 |
// |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
// |                 |                 |                 |
//  Premier domaine    deuxième domaine  troisième domaine
//  MPI
//  <--------------->
//    rank_length
//  |                |
// min              max
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <unistd.h>

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
    
    // Paramètres globaux

    double pi                   = 3.1415926535897932;        // Le nombre pi
    int discretization_per_rank = 100000;                      // Discrétisation par processus
    double rank_length          = 0.5*pi / number_of_ranks;  // Taille d'un rang

    // Paramètres locaux

    double min   = rank * rank_length;                            // Minimum local du rang
    double max   = (rank+1) * rank_length;                        // Maximum local du rang
    double delta = rank_length / discretization_per_rank;         // Taille d'un rectangle
    double x;                                                     // Variable d'intégration
    double local_integration;                                     // Intégration dans chaque rang
    double integration;                                           // Valeur finale de l'intégration
    int ierror;                                                   // Code d'erreur pour MPI
    
    // On affiche à l'écran quelques paramètres

    std::cout << " Le rang " << rank << " s'occupe de la portion comprise entre " << min << " et " << max << std::endl;

    usleep(100);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) std::cout << std::endl;
    
    // Intégration locale (chaque processus traite sa partie)

    for (int i = 0 ; i < discretization_per_rank ; i++) {
        x = min + (i-0.5)*delta ;
        local_integration = local_integration + sin(x)*delta ;
    }

    std::cout << " Le rang " << rank << " a pour intégration locale " << local_integration << std::endl;
    
    usleep(100);
    
    // Réduction dans `integration` pour avoir la valeur finale

    ierror = MPI_Reduce(&local_integration,   // valeur local à réduire
                    &integration,         // valeur finale
                    1,                   // Nombre d'élèments
                    MPI_DOUBLE,          // Type de donnée échangé
                    MPI_SUM,             // Type de réduction
                    0,                   // Rang de destination de la réduction
                    MPI_COMM_WORLD       // Communicateur
                );
    
    // Affichage du résultat depuis le rang 0

    if (rank == 0) {

        std::cout << std::endl;
        std::cout << " Le rang " << rank << " a pour résultat final " << integration << std::endl;

    }
    
    // Finalisation de MPI

    MPI_Finalize();
}
