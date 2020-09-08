// ______________________________________________________________________________
//
// Exercice 3 : gestion des zones critiques
//
// Dans cet exercice, vous apprendrez :
// - à utiliser une zone critique
//
// ______________________________________________________________________________

#include <iostream>
#include <cmath>
#include "omp.h"

int main( int argc, char *argv[] )
{

    int N = 100000000;
    double sum;
    
    #pragma omp parallel default(shared)
    {
        
        int id = omp_get_thread_num();
        
        double time_0 = omp_get_wtime();
    
        // Chaque thread ajoute à sum sa contribution N fois
        for (int i = 0 ; i < N ; i++) {
            sum += cos(2*id*i);
        }

        double time_1 = omp_get_wtime();
        
        std::cout << " Thread: " << id
                  << " - temps : " << time_1 - time_0 << " s" << std::endl;
        
    }
    
    std::cout << " Sum: " << sum << std::endl;
    
}
