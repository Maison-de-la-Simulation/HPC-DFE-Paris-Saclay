// ______________________________________________________________________________
//
// Exercice 1 : Les tout premiers pas OpenMP
//
// Dans cet exercice, vous apprendrez :
// - à créer une région parallèle avec OpenMP
// - à utiliser quelques fonctions de la bibliothèque
// - à utiliser la notion de variables privées ou partagées
// ______________________________________________________________________________

#include <iostream>

// On fait appel à la bibliothèque OpenMP spécifique
#include <omp.h>

int main( int argc, char *argv[] )
{
    // Initialisation des variables
    // Fonction pour récupérer le nombre de thread physique
    int nombre_de_threads_physiques =  omp_get_num_procs();
    // Utilisation de la fonction spécifique pour récupérer le nombre maximal de thread possible
    int nombre_max_de_threads = omp_get_max_threads();
    
    int thread_id;
    
    std::cout << "Nombre maximal de thread openmp : " << nombre_max_de_threads << std::endl;
    std::cout << "Nombre de threads physiques : " << nombre_de_threads_physiques << std::endl;
    
    #pragma omp parallel private(thread_id) default(shared)
    {
        // Utilisation de la fonction spécifique pour récupérer le nombre actif de threads
        int nombre_de_threads = omp_get_num_threads();
        
        // Identifiant du thread courant
        thread_id = omp_get_thread_num();
        
        std::cout << "Thread : " << thread_id << " / " << nombre_de_threads-1 << std::endl;
    }
    
}
