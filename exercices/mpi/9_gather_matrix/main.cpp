// ______________________________________________________________________________
//
// Exercice 9 :
//
// Cet exercice consiste à appliquer plusieurs concepts afin de réduire dans
// rang unique des informations partielles dispersées sur plusieurs rangs.
//
// ______________________________________________________________________________

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <iomanip>

// il est important d'inclure la bibliothèque
// sans quoi aucun de vos appels ne sera reconnu.
#include <mpi.h>

int main( int argc, char *argv[] )
{

    // Cette première fonction permet d'initialiser MPI,
    // elle est à appeler avant tout autre action avec MPI.

    MPI_Init( &argc, &argv );

    // Cette subroutine permet de récupérer le nombre total de rangs dans
    // le communicateur par défaut MPI_COMM_WORLD.

    int number_of_ranks;

    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );

    // Cette subroutine permet de récupérer le numéro du rang du processus
    // qui appelle cette subroutine dans
    // le communicateur par défaut MPI_COMM_WORLD.

    // Creation de la topology cartésienne

    int ranks_per_direction[2] = {3, 4};
    int periodicity[2] = {1, 1};
    int reorganisation = 1;
    MPI_Comm cartesian_communicator;

    MPI_Cart_create(???,
                    ???,
                    ???,
                    ???,
                    ???,
                    ???);

    // On récupère le rang dans le nouveau communicateur cartésien

    int rank;

    MPI_Comm_rank( cartesian_communicator, &rank );

    // On récupère les coordonnées du processus courant dans
    // la topologie cartésienne

    int rank_coordinates[2];

    MPI_Cart_coords( ???, ???, ???, ???);


	// Initialisation des blocs

	int nloc[2] = {3, 5};

	double * bloc = new double[nloc[0]*nloc[1]];

	// Chaque rang initialise son bloc

	for (int iy = 0 ; iy < nloc[1] ; iy++) {
		for (int ix = 0 ; ix < nloc[0] ; ix++) {
			bloc[iy * nloc[0] + ix] = rank;
		}
	}

	// Taille de la matrix

	int nglobal[2];
	nglobal[0] = nloc[0]*ranks_per_direction[0];
	nglobal[1] = nloc[1]*ranks_per_direction[1];

	double * all_blocs = new double[nglobal[0]*nglobal[1]];
	double * matrix = new double[nglobal[0]*nglobal[1]];

    // Communication des blocs au rang 0

    MPI_Gather(???, ???, ???, ???, ???, ???, ???, ???);

    // Affichage des blocs

    if (rank == 0) {

        std::cout << std::endl;
		std::cout <<  " nglobal : " << nglobal[0] << " " << nglobal[1] << std::endl;
        std::cout <<  " All blocs : "<< std::endl;

        for (int irank =0; irank < number_of_ranks ; irank++) {
            for(int i = 0; i < nloc[0]*nloc[1] ; i++) {
                std::cout << " " << std::setw(3) << all_blocs[irank*nloc[0]*nloc[1] + i] ;
            }
            std::cout << std::endl;
        }
    }

	// Création de la matrice finale

    if (rank == 0) {

		int start_index;

		for (int irank=0 ; irank < number_of_ranks ; irank++) {

			MPI_Cart_coords( ???, ???, ???, ???);

			// Rassemblez les blocs dans la matrice finale
			// ....

		}

	}

    // Affichage de la matrice

    if (rank == 0) {

        std::cout << std::endl;
		std::cout <<  " nglobal : " << nglobal[0] << " " << nglobal[1] << std::endl;
        std::cout <<  " Matrix : "<< std::endl;

        for(int iy = 0; iy < nglobal[1] ; iy++) {
            for(int ix = 0; ix < nglobal[0] ; ix++) {

                std::cout << " " << std::setw(3) << matrix[iy*nglobal[0]+ ix] ;

            }
            std::cout << std::endl;
        }
    }

    // Construction de la carte de la topologie  à partir de MPI_Cart_coords :

	delete [] bloc;
	delete [] matrix;
	delete [] all_blocs;

    MPI_Finalize();
}
