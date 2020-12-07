// ______________________________________________________________________________
//
// Exercice 2 : Parallélisation d'une boucle avec openMP
//
// Dans cet exercice, vous apprendrez :
// - à distribuer la charge d'une boucle for dans une région parallèle
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

// On fait appel à la bibliothèque OpenMP spécifique
#include <omp.h>

int main( int argc, char *argv[] )
{

    int N = 100000000;

    double cst = 15.8;
    
    double * A = new double[N];
    double * B = new double[N];
    double * C = new double[N];
    double * D = new double[N];
    
    double time_1, time_2;
    
    #pragma omp parallel default(shared) private(time_1,time_2)
    {
    
        #pragma omp master
        {
            time_1 = omp_get_wtime();
        }
        
        // Initialisation des tableaux
        #pragma omp for schedule(runtime)
        for (int i = 0 ; i < N ; i++) {
            B[i] = cst*cos(2*i);
            C[i] = cst*sin(3*i);
            D[i] = cst*cos(4*i)*cos(i);
        }
        
        // Boucle à paralléliser
        #pragma omp for schedule(runtime)
        for (int i = 0 ; i < N ; i++) {
            A[i] = B[i]*B[i] + C[i]*C[i]*C[i] + B[i]*C[i]*D[i] + D[i]*D[i]*D[i]*D[i];
        }
        
        #pragma omp master
        {
            time_2 = omp_get_wtime();
            std::cout << "Temps : " << time_2 - time_1 << " s" << std::endl;
        }
    
    }
    
    delete [] A;
    delete [] B;
    delete [] C;
    delete [] D;
    
}
