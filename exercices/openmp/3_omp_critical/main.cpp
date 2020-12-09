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
#include <iomanip>

int main( int argc, char *argv[] )
{

    int N = 1000;
    double sum = 0;
    
    #pragma omp parallel default(shared)
    {
        
        int id = omp_get_thread_num();
        //int nthreads = omp_get_num_threads();
        
        //int Nlocal=N/nthreads;
        
        double time_0 = omp_get_wtime();
    
        // Chaque thread ajoute à sum sa contribution N fois
        
            for (int i = 0 ; i < N ; i++) {
                sum += 2*(id+1)*i;
            }

        double time_1 = omp_get_wtime();
        
        std::cerr << " Thread: " << id
                  << " - temps : " << time_1 - time_0 << " s" << std::endl;
        
    }
    
    std::cerr << std::setprecision(15) << " Sum: " << sum << std::endl;
    
}
