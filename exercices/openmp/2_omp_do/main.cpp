// ______________________________________________________________________________
//
// Exercice 2 : Parallélisation d'une boucle avec openMP
//
// Dans cet exercice, vous apprendrez :
// - à distribuer la charge d'une boucle DO dans une région parallèle
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

int main( int argc, char *argv[] )
{

    int N = 100000000;

    double cst = 15.8;
    
    double * A = new double[N];
    double * B = new double[N];
    double * C = new double[N];
    double * D = new double[N];
    
    time_t time_1 = time(NULL);
    
    // Initialisation des tableaux
    for (int i = 0 ; i < N ; i++) {
        B[i] = cst*cos(2*i);
        C[i] = cst*sin(3*i);
        D[i] = cst*cos(4*i)*cos(i);
    }
    
    // Boucle à paralléliser
    for (int i = 0 ; i < N ; i++) {
        A[i] = B[i]*B[i] + pow(C[i],3) + B[i]*C[i]*D[i];
    }
    
    time_t time_2 = time(NULL);
    
    std::cout << "Temps : " << time_2 - time_1 << " s" << std::endl;
    
    delete [] A;
    delete [] B;
    delete [] C;
    delete [] D;
    
}
