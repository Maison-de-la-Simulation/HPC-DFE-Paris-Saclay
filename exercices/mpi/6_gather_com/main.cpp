// ______________________________________________________________________________
//
// Exercice 6 : Utilisation des communications collectives MPI_Gather
//
//
// Dans cet exercice, chaque rang MPI alloue et initie un tableau d'une taille
// déterminée de manière aléatoire.
// Le but est de rappatrier chaque morceau de tableau vers le rang 0.
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
    // ______________________________________________________
    // Initialisation de MPI
    
    MPI_Init( &argc, &argv );
    
    // Nombre total de rangs
    
    int number_of_ranks;
    
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );
    
    // Rang du processus MPI
    
    int rank;
    
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    // _______________________________________________________
    // Paramètres
    
    srand (rank);
    
    // The size is drawn between 1 and 100
    int size = rand() % 99 + 1;
    
    double * array = new double [size];
    
    // _______________________________________________________
    // Initilisation
    
    for (int i = 0; i < size ; i++) {
        array[i] = drand48()*(rank+1);
    }
    
    // _______________________________________________________
    // On rapatrie toutes les données sur le rang 0
    
    int * local_size = new int[number_of_ranks];
    
    // Le rang 0 récupère d'abord combien il y a d'éléments dans chaque tableau de chaque rang
    MPI_???
    
    // On calcule le nombre total d'éléments sur le rang 0
    
    int total_size = 0;
    
    if (rank == 0) {
    
        ???
        
        std::cout << " Le nombre total d'éléments est de " << total_size << "." << std::endl;
        for (int i = 0 ; i < number_of_ranks ; i++) {
            std::cout << " Le rang " << i << " possède " << local_size[i] << " éléments." << std::endl;
        }
    }
    
    // On prépare le tableau des déplacements pour le MPI_Gatherv
        
    int * displacement = new int [number_of_ranks];
        
    ???
    
    // On ramène tout le monde sur le rang 0
    
    double * global_array = new double [total_size];
    
    MPI_???
    
    // On somme tous les éléments sur 0 et on affiche le résultat
    
    double sum = 0;
    
    for (int i = 0 ; i < total_size ; i++) {
        sum += global_array[i];
    }
    
    if (rank == 0) std::cout << " La somme calculée sur le rang 0 est de : " << sum << "." << std::endl;
    
    // On somme tous les éléments en utilisant MPI_Reduce
    
    double local_sum = 0;
    
    ???
    
    MPI_???
    
    if (rank == 0) std::cout << " La somme calculée avec MPI_Reduce est de : " << sum << "." << std::endl;
    
    // _______________________________________________________
    // Finalisation de MPI

    MPI_Finalize();
}
