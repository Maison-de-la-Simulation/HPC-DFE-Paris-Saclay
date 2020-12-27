// ______________________________________________________________________________
//
// Exercice 4 : Utilisation des réductions
//
//
// Dans cet exercice, nous intégrons la fonction f(x) = sin(x) de
// manière numérique entre 0 et pi/2 et de manière parallèle
// On utilise une intégration par la méthode des rectangles.
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <iomanip>

#include "omp.h"

int main( int argc, char *argv[] )
{
    
    
    
    // Paramètres globaux

    double pi                   = 3.1415926535897932;        // Le nombre pi
    int discretization          = 100000;                      // Discrétisation par processus
    double min = 0;
    double max = 0.5*pi;

    // Paramètres locaux


    double delta = (max - min) / discretization;         // Taille d'un rectangle
    double x;                                                     // Variable d'intégration
    double integration = 0;                                           // Valeur finale de l'intégration
    
    double time_1 = omp_get_wtime();

    // Intégration
    #pragma omp parallel for reduction(+:integration) default(shared) private(x) firstprivate(delta)
    for (int i = 0 ; i < discretization ; i++) {
        x = min + (i+0.5)*delta ;
        integration += sin(x)*delta ;
    }
    
    double time_2 = omp_get_wtime();

    std::cout << std::setprecision(15) << "Intégration: " << integration
            << " - Temps: " << time_2 - time_1
           << std::endl;


}
