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

// il est important d'inclure la bibliothèque
// sans quoi aucun de vos appels ne sera reconnu.
#include <mpi.h>

int main( int argc, char *argv[] )
{
    
    
    
    // Paramètres globaux

    double pi                   = 3.1415926535897932;        // Le nombre pi
    int discretization          = 1000;                      // Discrétisation par processus
    double min = 0;
    double max = 0.5*pi;

    // Paramètres locaux


    double delta = (max - min) / discretization;         // Taille d'un rectangle
    double x;                                                     // Variable d'intégration
    double integration = 0;                                           // Valeur finale de l'intégration
    

    // Intégration

    for (int i = 0 ; i < discretization ; i++) {
        x = min + (i+1-0.5)*delta ;
        integration += sin(x)*delta ;
    }
    

    std::cout << "Intégration: " << integration << std::endl;


}
