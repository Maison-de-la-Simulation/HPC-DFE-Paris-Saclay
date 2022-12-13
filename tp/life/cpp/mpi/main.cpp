/* ______________________________________________________________________________

  Advanced Game of Life

! ______________________________________________________________________________ */

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
//#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <iomanip>
#include <sstream>
#include <mpi.h>

// function to check if the cell is located outside the domain
bool outside_domain(int origin_x, int origin_y, int ix, int iy, int nx, int ny) {
	if (origin_x + ix < 1 || origin_x + ix > nx-2 || origin_y + iy < 1 || origin_y + iy > ny-2 ) {
		return true;
	} else {
		return false;
	}
}

int main( int argc, char *argv[] )
{

    // Parameters ____________________________________________________________________________________

	int nx;   // number of cells in the x direction without boundaries
	int ny;   // number of cells in the y direction without boundaries

	int iterations;  // number of iterations

	int diag_period; // diagnostic output period

	int print_period; // print period

	double rabbit_migration_success;  // probability for a rabbit to change cell
	double fox_migration_success;     // probability for a fox to change cell
	double seed_success;              // probability to have a new plant on a cell
	double hunting_success;           // probability for a fox to catch a rabbit
	int max_babies_per_rabbits;       // Maximul number of babies a rabbit can have
	int max_babies_per_fox;
	int max_foxes_per_cell;
	double max_plants_per_cell;
	double rabbits_hunger;            // How much plant eat a rabbit per iteration
	double fox_sens_of_smell;
	double fox_non_sociability;
	double plant_growing_velocity;

	int number_of_ranks;        // Number of MPI ranks
	int rank;                   // Number of MPI ranks
	int ranks_per_direction[2]; // Nnumber of ranks for each direction : x, y
	int periodicity[2]; // Nnumber of ranks for each direction : x, y, z
	int reorganisation;

	MPI_Comm cartesian_communicator;
	MPI_Request request[8];
	int rank_coordinates[2];
	int tag = 0;

	int nx_loc; // local number of cells in the x direction per rank
	int ny_loc; // local number of cells in the y direction per rank

	int origin_x; // Origin of the rank in the global index space
	int origin_y; // Origin of the rank in the global index space

	int rank_neighbors_mx;
	int rank_neighbors_px;

	int rank_neighbors_my;
	int rank_neighbors_py;

    // MPI initilization _____________________________________________________________________________

    // Initilization

    MPI_Init( &argc, &argv );

    // We get the number of ranks

    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );

    // We get the rank of the current MPI process

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	// Default configuration __________________________________________________________________________

	nx = 96;
	ny = 84;
	iterations = 1000;
	diag_period = 10;
	print_period = 100;

	// plants properties
	seed_success = 0.;
	max_plants_per_cell = 10.;
	plant_growing_velocity = 0.1;

	// rabbits properties
	rabbits_hunger      = 0.5;
	max_babies_per_rabbits = 5;
	rabbit_migration_success = 0.5;

	// fox properties
	hunting_success       = 0.5;
	fox_sens_of_smell     = 2.;
	max_foxes_per_cell    = 5;
	fox_migration_success = 0.8;
	fox_non_sociability   = 0.1;
	max_babies_per_fox    = 3;

	// MPI properties
	ranks_per_direction[0] = 2;
	ranks_per_direction[1] = 2;
	periodicity[0] = 1;
	periodicity[1] = 1;
	reorganisation = 0;

	// Command line arguments __________________________________________________

	if (argc > 1) {
		unsigned int iarg = 1;
		std::string key;
		while(iarg < argc) {
			key = argv[iarg];
			if (key == "-it") {
				iterations = atoi(argv[iarg+1]);
				iarg++;
			} else if (key == "-ny") {
				ny = atoi(argv[iarg+1]);
				iarg++;
			} else if (key == "-nx") {
				nx= atoi(argv[iarg+1]);
				iarg++;
			} else if (key == "-d") {
				diag_period= atoi(argv[iarg+1]);
				iarg++;
			} else if (key == "-p") {
				print_period= atoi(argv[iarg+1]);
				iarg++;
			} else if (key == "-r") {
				ranks_per_direction[1]= atoi(argv[iarg+1]);
				ranks_per_direction[0]= atoi(argv[iarg+2]);
				iarg+=2;
			} else {
				iarg++;
			}
		}
	}

	if (rank==0) {
		// Creation of the diag folder
		system("mkdir -p diags");
	}

	// Creation of the cartesian topology

	if (number_of_ranks != ranks_per_direction[0] * ranks_per_direction[1]) {
		std::cerr << " CONFIGURATION ERROR: the total number of patches must match the topology: "
				  << number_of_ranks << " " << ranks_per_direction[0] << " " << ranks_per_direction[1]  << std::endl;
		exit(0);
	}

	MPI_Cart_create(MPI_COMM_WORLD,
                         2,
                         ranks_per_direction,
                         periodicity,
                         reorganisation,
                         &cartesian_communicator);

	MPI_Comm_rank( cartesian_communicator, &rank );

	MPI_Cart_coords( cartesian_communicator, rank, 2, rank_coordinates);

	// On récupère les rangs voisins

	// en -y (rank_neighbors_my), en +y (rank_neighbors_py)

	MPI_Cart_shift( cartesian_communicator, 0, 1, &rank_neighbors_my, &rank_neighbors_py);

	// En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)

	MPI_Cart_shift( cartesian_communicator, 1, 1, &rank_neighbors_mx, &rank_neighbors_px);

	// Computation of the local (rank-level) number of cells
	nx_loc = nx / ranks_per_direction[1];
	ny_loc = ny / ranks_per_direction[0];

	// Origin of each rank in the global indexing
	origin_x = rank_coordinates[1] * nx_loc;
	origin_y = rank_coordinates[0] * ny_loc;

	// We add the boundaries and the ghost cells
	nx_loc += 2;
	ny_loc += 2;

	// Arrays and additional parameters

	double * plants = new double[ny_loc*nx_loc];
	double * new_plants = new double[ny_loc*nx_loc];

	int * rabbits = new int[ny_loc*nx_loc];
	int * new_rabbits = new int[ny_loc*nx_loc];

	int * foxes = new int[ny_loc*nx_loc];
	int * new_foxes = new int[ny_loc*nx_loc];

	double sum_loc;
	double sum_loc_2;

	double * sum_plants_before_rabbits = new double[iterations];
	double * sum_plants_after_rabbits = new double[iterations];
	double * sum_rabbits_before_foxes = new double[iterations];
	double * sum_foxes = new double[iterations];

	int ix;
	int iy;
	int ir;
	int icell;
	int icell2;
	int i;
	int ifox;
	int iteration;
	int shiftx;
	int shifty;

	double random_number;
	double threshold;
	double score_sum;
	double score[9];

	double temporary_time;
	int shiftx_list[9];
	int shifty_list[9];

	for (shifty = -1 ; shifty < 2 ; shifty++) {
		for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
			icell = (shifty+1)*3 + shiftx+1;
			shiftx_list[icell] = shiftx;
			shifty_list[icell] = shifty;
		}
	}

	// timers
	double timer_init = 0.;          // For initialization
	double timer_main_loop = 0.;	 // For main loop
	double timer_com = 0.;	         // For point-to-point communication
	double timer_global_com = 0.;	 // For global communication
	double timer_comp = 0.;	         // For computation
	double timer_temp = 0.;

	// Creation du type dérivé pour les communications des ghost cells
    MPI_Datatype type_column_double;
    MPI_Type_vector(ny_loc, 1, nx_loc, MPI_DOUBLE, &type_column_double);
    MPI_Type_commit(&type_column_double);

	MPI_Datatype type_row_double;
	MPI_Type_contiguous(nx_loc, MPI_DOUBLE, &type_row_double);
	MPI_Type_commit(&type_row_double);

	MPI_Datatype type_column_int;
    MPI_Type_vector(ny_loc, 1, nx_loc, MPI_INT, &type_column_int);
    MPI_Type_commit(&type_column_int);

	MPI_Datatype type_row_int;
    MPI_Type_contiguous(nx_loc, MPI_INT, &type_row_int);
    MPI_Type_commit(&type_row_int);

	// Creation du type dérivé pour les diags

	MPI_Datatype bloc_double;
	MPI_Type_vector((ny_loc-2), (nx_loc-2), nx_loc, MPI_DOUBLE, &bloc_double);
	MPI_Type_commit(&bloc_double);

	MPI_Datatype bloc_int;
	MPI_Type_vector((ny_loc-2), (nx_loc-2), nx_loc, MPI_INT, &bloc_int);
	MPI_Type_commit(&bloc_int);

	// Terminal output summary _________________________________________________

	if (rank==0) {
		std::cout << " ------------------------------------ "<< std::endl;
		std::cout << " GAME OF LIFE"<< std::endl;
		std::cout << " ------------------------------------ "<< std::endl;

		std::cout << "  - nx: " << nx-2 << std::endl;
		std::cout << "  - ny: " << ny-2 << std::endl;
		std::cout << "  - iterations: " << iterations << std::endl;
		std::cout << std::endl;
		std::cout << "  Rabbits properties: " << std::endl;
		std::cout << "  - rabbits hunger: " << rabbits_hunger << std::endl;
		std::cout << std::endl;
		std::cout << "  Fox properties: " << std::endl;
		std::cout << "  - fox sens of smell: " << rabbits_hunger << std::endl;
		std::cout << std::endl;
		std::cout << "  MPI properties: " << std::endl;
		std::cout << "  - number of ranks: " << number_of_ranks << std::endl;
		std::cout << "  - number of ranks in x: " << ranks_per_direction[1] << std::endl;
		std::cout << "  - number of ranks in y: " << ranks_per_direction[0] << std::endl;
		std::cout << "  - local number of cells in x: " << nx_loc-2 << std::endl;
		std::cout << "  - local number of cells in y: " << ny_loc-2 << std::endl;

		std::cout << std::endl;
	}

	// Initialization __________________________________________________________

	timer_init = MPI_Wtime();

	// random generator
	srand (timer_init * rank);
	//srand(0);

	for (i = 0 ; i < ny_loc*nx_loc ; i++) {
		plants[i] = 0.;
		new_plants[i] = 0.;
		rabbits[i] = 0;
		new_rabbits[i] = 0;
		foxes[i] = 0;
		new_foxes[i] = 0;
	}

	// if (rank == 0) {
	// 	plants[ int((0.5*ny_loc + 0.5)*nx_loc) ] = 10;
	// 	rabbits[ int((0.5*ny_loc + 0.5)*nx_loc) ] = 10;
	// 	foxes[ int((0.5*ny_loc + 0.5)*nx_loc) ] = 10;
	// }

	for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		for (ix = 1 ; ix < nx_loc-1 ; ix++) {
			plants[iy*nx_loc + ix] = 1.;
		}
	}

	int amplitude = 2;
	for (ir = 0 ; ir < 2 ; ir++) {
		shiftx = rand()%(2*amplitude+1)-amplitude;
		shifty = rand()%(2*amplitude+1)-amplitude;
		ix = rand()%(nx_loc-2*(amplitude+1)) + amplitude + 1;
		iy = rand()%(ny_loc-2*(amplitude+1)) + amplitude + 1;
		rabbits[iy*nx_loc + ix] = rand()%5 + 5;
		foxes[(iy+shifty)*nx_loc + (ix+shiftx)] = rand()%5 + 5;
	}

	//rabbits[int(0.5*ny)*nx + int(0.5*nx)] = 5;

	// for (ir = 0 ; ir < 5 ; ir++) {
	// 	ix = rand()%(nx-2) + 1;
	// 	iy = rand()%(ny-2) + 1;
	// 	rabbits[iy*nx + ix] = rand()%5 + 5;
	// }

	// Main loop _______________________________________________________________

	if (rank==0) {
		std::cout << " ------------------------------------ "<< std::endl;
		std::cout << " MAIN LOOP"<< std::endl;
		std::cout << " ------------------------------------ "<< std::endl;
	}

	timer_main_loop = MPI_Wtime();
	timer_init = timer_main_loop - timer_init;

	for (iteration = 0 ; iteration < iterations ; iteration++) {

		// Step 1 - plants grow and propagate

		timer_temp = MPI_Wtime();

		for (i = 0 ; i < ny_loc*nx_loc ; i++) {
			new_plants[i] = 0;
		}

		// for (iy = 0 ; iy < ny_loc ; iy++) {
		// 	for (ix = 0 ; ix < nx_loc ; ix++) {
		//
		// 		if (plants[iy*nx_loc + ix] > 0) {
		//
		// 			// if (outside_domain(origin_x, origin_y, ix+1, iy, nx, ny)) {
		// 			// 	new_plants[(iy)*nx_loc + (ix)] = 0.;
		// 			// } else {
		//
		// 			if (iy+1 < ny_loc-1) {
		// 				new_plants[(iy+1)*nx_loc + ix] = plants[iy*nx_loc + ix];
		// 				plants[iy*nx_loc + ix] = 0;
		// 			}
		//
		// 			// }
		// 		}
		// 	}
		// }

		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {

				// We check if the cell ix, iy has pants around or inside the cell

				bool has_plants_around = false;

				for (shifty = -1 ; shifty < 2 ; shifty++) {
					for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
						if (plants[(iy+shifty)*nx_loc + (ix+shiftx)] > 0) {
							has_plants_around = true;
						}
					}
				}

				if (has_plants_around) {

					// Plants propagation or growth

					new_plants[iy*nx_loc + ix] = std::min(max_plants_per_cell, plants[iy*nx_loc + ix] + plant_growing_velocity);
					//new_plants[iy*nx + ix] = plants[iy*nx + ix] + 1.;

				} else {

					// Seed growth

					random_number = rand()/(double)RAND_MAX;

					if (random_number < seed_success) {
						new_plants[iy*nx_loc + ix] += plant_growing_velocity;
					}

				}

			}
		}

		// Update of plants
		for (i = 0 ; i < ny_loc*nx_loc ; i++) {
			plants[i] = new_plants[i];
		}

        timer_comp += MPI_Wtime() - timer_temp;

		// Ghost cells communication

		timer_temp = MPI_Wtime();

		// Receive from -x and send to +x
		MPI_Sendrecv(&plants[nx_loc-2],
				1,
				type_column_double,
				rank_neighbors_px,
				tag,
				&plants[0],
				1,
				type_column_double,
				rank_neighbors_mx,
				tag,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		//MPI_Irecv(&plants[0], 1, type_column_double, rank_neighbors_mx, tag, cartesian_communicator, &request[0]);
		//MPI_Isend(&plants[nx_loc-2], 1, type_column_double, rank_neighbors_px, tag, cartesian_communicator, &request[1]);

		// Receive from +x and send to -x
		MPI_Sendrecv(&plants[1],
				1,
				type_column_double,
				rank_neighbors_mx,
				tag,
				&plants[nx_loc-1],
				1,
				type_column_double,
				rank_neighbors_px,
				tag,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&plants[nx_loc-1], 1, type_column_double, rank_neighbors_px, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&plants[1], 1, type_column_double, rank_neighbors_mx, tag, cartesian_communicator, &request[3]);

		//MPI_Waitall(4,request,MPI_STATUS_IGNORE);

		// Receive from -y and send to +y
		MPI_Sendrecv(&plants[(ny_loc-2) * nx_loc],
				1,
				type_row_double,
				rank_neighbors_py,
				tag,
				&plants[0],
				1,
				type_row_double,
				rank_neighbors_my,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&plants[0], 1, type_row_double, rank_neighbors_my, tag, cartesian_communicator, &request[0]);
		// MPI_Isend(&plants[(ny_loc-2) * nx_loc], 1, type_row_double, rank_neighbors_py, tag, cartesian_communicator, &request[1]);

		// Receive from +y and send to -y
		MPI_Sendrecv(&plants[nx_loc],
				1,
				type_row_double,
				rank_neighbors_my,
				tag,
				&plants[(ny_loc-1) * nx_loc],
				1,
				type_row_double,
				rank_neighbors_py,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&plants[(ny_loc-1) * nx_loc], 1, type_row_double, rank_neighbors_py, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&plants[1], 1, type_row_double, rank_neighbors_my, tag, cartesian_communicator, &request[3]);

		// MPI_Waitall(4,request,MPI_STATUS_IGNORE);

		timer_com += MPI_Wtime() - timer_temp;

		timer_temp = MPI_Wtime();

		// Reduction
		sum_loc = 0;
		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {
				sum_loc += new_plants[iy*nx_loc + ix];
			}
		}

		timer_comp += MPI_Wtime() - timer_temp;

		// MPI reduction in the rank 0

		timer_temp = MPI_Wtime();

		MPI_Reduce(&sum_loc,   // valeur local à réduire
				&sum_plants_before_rabbits[iteration],         // valeur finale
				1,                   // Nombre d'élèments
				MPI_DOUBLE,          // Type de donnée échangé
				MPI_SUM,             // Type de réduction
				0,                   // Rang de destination de la réduction
				cartesian_communicator       // Communicateur
		);

		timer_global_com += MPI_Wtime() - timer_temp;


		// Step 2 - Rabbits

		timer_temp = MPI_Wtime();

		// Reinitialize new rabbits before
		for (i = 0 ; i < ny_loc*nx_loc ; i++) {
			new_rabbits[i] = 0;
		}

		// for (iy = 0 ; iy < ny_loc ; iy++) {
		// 	for (ix = 0 ; ix < nx_loc ; ix++) {
		//
		// 		if (rabbits[iy*nx_loc + ix] > 0) {
		//
		// 			// if (outside_domain(origin_x, origin_y, ix+1, iy, nx, ny)) {
		// 			// 	new_plants[(iy)*nx_loc + (ix)] = 0.;
		// 			// } else {
		//
		// 			if (ix+1 < nx_loc-1) {
		// 				new_rabbits[(iy)*nx_loc + ix+1] = rabbits[iy*nx_loc + ix];
		// 				rabbits[iy*nx_loc + ix] = 0;
		// 			}
		//
		// 			// }
		// 		}
		// 	}
		// }

		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	for (ix = 1 ; ix < nx_loc-1 ; ix++) {
		//
		// 		// We check if the cell ix, iy has pants around or inside the cell
		//
		// 		bool has_plants_around = false;
		//
		// 		for (shifty = -1 ; shifty < 2 ; shifty++) {
		// 			for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
		// 				if (rabbits[(iy+shifty)*nx_loc + (ix+shiftx)] > 0.) {
		// 					has_plants_around = true;
		// 				}
		// 			}
		// 		}
		//
		// 		if (has_plants_around) {
		//
		// 			// Plants propagation or growth
		//
		// 			new_rabbits[iy*nx_loc + ix] = std::min((double) 10., rabbits[iy*nx_loc + ix] + 1.);
		// 			//new_plants[iy*nx + ix] = plants[iy*nx + ix] + 1.;
		//
		// 		}
		//
		// 	}
		// }

		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {

				for (ir = 0 ; ir < rabbits[iy*nx_loc + ix] ; ir++) {

					if (new_plants[iy*nx_loc + ix] >= 1.) {

						// if there is enough food, the rabbits eat and make babies at the same location

						new_plants[iy*nx_loc + ix] -= rabbits_hunger;
						new_rabbits[iy*nx_loc + ix] += 1 + rand()%(max_babies_per_rabbits+1);

					} else {

						// all the plant has been eaten
						new_plants[iy*nx_loc + ix] = 0.;

						// the rabbit has to migrate to another place
						// It has a certain chance to succeed else it dies

						random_number = rand()/(double)RAND_MAX;

						if (random_number <= rabbit_migration_success) {

							// -------------------------------------
							// Method 1 - Rabbits migrate according to the food and foxes around

							score_sum = 0;

							for (shifty = -1 ; shifty < 2 ; shifty++) {
								for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
									icell = (shifty+1)*3 + shiftx+1;
									if (outside_domain(origin_x, origin_y, ix+shiftx, iy+shifty, nx, ny)) {
										score[icell] = 0.;
									} else {
										icell2 = (iy+shifty)*nx_loc + shiftx+ix;
										score[icell] = std::max(plants[icell2] * 2. - foxes[icell2] * 2. + rabbits[icell2]*0.1 ,1.);
									}
									score_sum += score[icell];
								}
							}

							score_sum = 1./score_sum;

							random_number = rand()/(double)RAND_MAX;

							threshold = score[0]*score_sum;
							icell = 0;

							while (random_number > threshold ) {
								icell += 1;
								threshold += score[icell]*score_sum;
							}

							new_rabbits[(iy+shifty_list[icell])*nx_loc + (ix+shiftx_list[icell])] += 1;

							// -------------------------------------
							// Method 2 - Rabbits migrate randomly

								// do {
								// 	shiftx = rand()%3-1;
								// 	shifty = rand()%3-1;
								//
								// } while ( outside_domain(ix+shiftx,iy+shifty,nx,ny) || ((shiftx == 0) && (shifty == 0)) );
								//
								// new_rabbits[(iy+shifty)*nx + (ix+shiftx)] += 1;


						}

					} // end if
				}
			}
		}

		// Array update
		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {
				rabbits[iy*nx_loc + ix] = new_rabbits[iy*nx_loc + ix];
				plants[iy*nx_loc + ix] = new_plants[iy*nx_loc + ix];
			}
		}

        timer_comp += MPI_Wtime() - timer_temp;

		// Ghost cells communication

		timer_temp = MPI_Wtime();

		// Receive from +x and send to -x
		MPI_Sendrecv(&plants[nx_loc-2],
				1,
				type_column_double,
				rank_neighbors_mx,
				tag,
				&plants[0],
				1,
				type_column_double,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);


		// double buffer_column_int_send[ny_loc];
		// double buffer_column_int_recv[ny_loc];

		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	buffer_column_int_send[iy] = plants[(nx_loc-2) + iy*nx_loc];
		// }
		//
		// // Receive from -x and send to +x
		// MPI_Sendrecv(&buffer_column_int_send[0],
		// 		ny_loc,
		// 		MPI_DOUBLE,
		// 		rank_neighbors_px,
		// 		tag,
		// 		&buffer_column_int_recv[0],
		// 		ny_loc,
		// 		MPI_DOUBLE,
		// 		rank_neighbors_mx,
		// 		MPI_ANY_TAG,
		// 		cartesian_communicator,
		// 		MPI_STATUS_IGNORE);
		//
		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	plants[iy*nx_loc] = buffer_column_int_recv[iy];
		// }

		// MPI_Irecv(&plants[0], 1, type_column_double, rank_neighbors_mx, tag, cartesian_communicator, &request[0]);
		// MPI_Isend(&plants[nx_loc-2], 1, type_column_double, rank_neighbors_px, tag, cartesian_communicator, &request[1]);

		// Receive from +x and send to -x
		MPI_Sendrecv(&plants[1],
				1,
				type_column_double,
				rank_neighbors_px,
				tag,
				&plants[nx_loc-1],
				1,
				type_column_double,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	buffer_column_int_send[iy] = plants[1 + iy*nx_loc];
		// }
		//
		// // Receive from -x and send to +x
		// MPI_Sendrecv(&buffer_column_int_send[0],
		// 		ny_loc,
		// 		MPI_DOUBLE,
		// 		rank_neighbors_px,
		// 		tag,
		// 		&buffer_column_int_recv[0],
		// 		ny_loc,
		// 		MPI_DOUBLE,
		// 		rank_neighbors_mx,
		// 		MPI_ANY_TAG,
		// 		cartesian_communicator,
		// 		MPI_STATUS_IGNORE);
		//
		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	plants[(nx_loc-1) + iy*nx_loc] = buffer_column_int_recv[iy];
		// }

		// MPI_Irecv(&plants[nx_loc-1], 1, type_column_double, rank_neighbors_px, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&plants[1], 1, type_column_double, rank_neighbors_mx, tag, cartesian_communicator, &request[3]);

		// Receive from -x and send to +x
		MPI_Sendrecv(&rabbits[nx_loc-2],
				1,
				type_column_int,
				rank_neighbors_px,
				tag,
				&rabbits[0],
				1,
				type_column_int,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[0], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[4]);
		// MPI_Isend(&rabbits[nx_loc-2], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[5]);

		// Receive from +x and send to -x
		MPI_Sendrecv(&rabbits[1],
				1,
				type_column_int,
				rank_neighbors_mx,
				tag,
				&rabbits[nx_loc-1],
				1,
				type_column_int,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[nx_loc-1], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[6]);
		// MPI_Isend(&rabbits[1], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[7]);

		// MPI_Waitall(8,request,MPI_STATUS_IGNORE);

		// Receive from -y and send to +y
		MPI_Sendrecv(&plants[(ny_loc-2) * nx_loc],
				1,
				type_row_double,
				rank_neighbors_py,
				tag,
				&plants[0],
				1,
				type_row_double,
				rank_neighbors_my,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&plants[0], 1, type_row_double, rank_neighbors_my, tag, cartesian_communicator, &request[0]);
		// MPI_Isend(&plants[(ny_loc-2) * nx_loc], 1, type_row_double, rank_neighbors_py, tag, cartesian_communicator, &request[1]);

		// Receive from +y and send to -y
		MPI_Sendrecv(&plants[nx_loc],
				1,
				type_row_double,
				rank_neighbors_my,
				tag,
				&plants[(ny_loc-1) * nx_loc],
				1,
				type_row_double,
				rank_neighbors_py,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&plants[(ny_loc-1) * nx_loc], 1, type_row_double, rank_neighbors_py, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&plants[1], 1, type_row_double, rank_neighbors_my, tag, cartesian_communicator, &request[3]);

		// Receive from -y and send to +y
		MPI_Sendrecv(&rabbits[(ny_loc-2) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				tag,
				&rabbits[0],
				1,
				type_row_int,
				rank_neighbors_my,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[0], 1, type_row_int, rank_neighbors_my, tag, MPI_COMM_WORLD, &request[4]);
		// MPI_Isend(&rabbits[(ny_loc-2) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, MPI_COMM_WORLD, &request[5]);

		// Receive from +y and send to -y
		MPI_Sendrecv(&rabbits[nx_loc],
				1,
				type_row_int,
				rank_neighbors_my,
				tag,
				&rabbits[(ny_loc-1) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[(ny_loc-1) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, MPI_COMM_WORLD, &request[6]);
		// MPI_Isend(&rabbits[1], 1, type_row_int, rank_neighbors_my, tag, MPI_COMM_WORLD, &request[7]);

		// MPI_Waitall(8,request,MPI_STATUS_IGNORE);

		timer_com += MPI_Wtime() - timer_temp;

		timer_temp = MPI_Wtime();

		// Reductions
		sum_loc = 0.;
		sum_loc_2 = 0.;
		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {
				sum_loc += (double)(new_rabbits[iy*nx_loc + ix]);
				sum_loc_2 += (double)(new_plants[iy*nx_loc + ix]);
			}
		}

		timer_comp += MPI_Wtime() - timer_temp;

		// MPI reduction in the rank 0

		timer_temp = MPI_Wtime();

		MPI_Reduce(&sum_loc,   // valeur local à réduire
				&sum_rabbits_before_foxes[iteration],         // valeur finale
				1,                   // Nombre d'élèments
				MPI_DOUBLE,          // Type de donnée échangé
				MPI_SUM,             // Type de réduction
				0,                   // Rang de destination de la réduction
				cartesian_communicator       // Communicateur
		);

		MPI_Reduce(&sum_loc_2,   // valeur local à réduire
				&sum_plants_after_rabbits[iteration],         // valeur finale
				1,                   // Nombre d'élèments
				MPI_DOUBLE,          // Type de donnée échangé
				MPI_SUM,             // Type de réduction
				0,                   // Rang de destination de la réduction
				cartesian_communicator       // Communicateur
		);

		timer_global_com += MPI_Wtime() - timer_temp;

		// Step 3 - Foxes

		timer_temp = MPI_Wtime();

		// Reinitialization of the new foxes
		for (i = 0 ; i < ny_loc*nx_loc ; i++) {
			new_foxes[i] = 0;
		}

		// for (iy = 0 ; iy < ny_loc ; iy++) {
		// 	for (ix = 0 ; ix < nx_loc ; ix++) {
		//
		// 		if (foxes[iy*nx_loc + ix] > 0) {
		//
		// 			// if (outside_domain(origin_x, origin_y, ix+1, iy, nx, ny)) {
		// 			// 	new_plants[(iy)*nx_loc + (ix)] = 0.;
		// 			// } else {
		//
		// 			if (iy+1 < ny_loc-1) {
		// 				new_foxes[(iy+1)*nx_loc + ix] = foxes[iy*nx_loc + ix];
		// 				foxes[iy*nx_loc + ix] = 0;
		// 			}
		//
		// 			// }
		// 		}
		// 	}
		// }


		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {

				// for each fox in the cell ix,iy
				for (ifox = 0 ; ifox < foxes[iy*nx_loc + ix] ; ifox++) {

					if (new_rabbits[iy*nx_loc + ix] >= 1 && foxes[iy*nx_loc + ix] <= max_foxes_per_cell) {

						// if there is enough food, the foxes hunt a rabbit
						// and make babies at the same location

						random_number = rand()/(double)RAND_MAX;

						if (random_number <= hunting_success) {

							new_rabbits[iy*nx_loc + ix] -= 1;
							new_foxes[iy*nx_loc + ix] += 1 + rand()%(max_babies_per_fox+1);

						} else {

							// the fox stays in the same cell
							new_foxes[iy*nx_loc + ix] += 1;

						}

					} else {

						// All the rabbits have been eaten or the fox density is too high
						// the fox has to migrate to another place
						// It has a certain chance to succeed else it dies

						random_number = rand()/(double)RAND_MAX;

						if (random_number <= fox_migration_success) {

							// -------------------------------------
							// Method 1 - Foxes migrate according to the food around

							score_sum = 0;

							for (shifty = -1 ; shifty < 2 ; shifty++) {
								for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
									icell = (shifty+1)*3 + shiftx+1;
									if (outside_domain(origin_x, origin_y, ix+shiftx, iy+shifty, nx, ny)) {
										score[icell] = 0.;
									} else {
										icell2 = (iy+shifty)*nx_loc + shiftx+ix;
										score[icell] = std::max(rabbits[icell2] * fox_sens_of_smell - foxes[icell2] * fox_non_sociability,1.);
									}
									score_sum += score[icell];
								}
							}

							score_sum = 1./score_sum;

							random_number = rand()/(double)RAND_MAX;

							threshold = score[0]*score_sum;
							icell = 0;

							while (random_number > threshold ) {
								icell += 1;
								threshold += score[icell]*score_sum;
							}

							new_foxes[(iy+shifty_list[icell])*nx_loc + (ix+shiftx_list[icell])] += 1;

							// -----------------------------------------------------
							// Method 2
							// 	do {
							// 		shiftx = rand()%3-1;
							// 		shifty = rand()%3-1;
							// 	} while ( outside_domain(ix+shiftx,iy+shifty,nx,ny) || ((shiftx == 0) && (shifty == 0)) );
							// 	new_foxes[(iy+shifty)*nx + (ix+shiftx)] += 1;
						}
					} // end if
				} // end ifox
			} //end ix
		} //end iy

		// Array update
		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {
				rabbits[iy*nx_loc + ix] = new_rabbits[iy*nx_loc + ix];
				foxes[iy*nx_loc + ix] = new_foxes[iy*nx_loc + ix];
			}
		}

		timer_comp += MPI_Wtime() - timer_temp;

		// Ghost cells communication

		timer_temp = MPI_Wtime();

		// Receive from -x and send to +x
		MPI_Sendrecv(&foxes[nx_loc-2],
				1,
				type_column_int,
				rank_neighbors_px,
				tag,
				&foxes[0],
				1,
				type_column_int,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&foxes[0], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[0]);
		// MPI_Isend(&foxes[nx_loc-2], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[1]);

		// Receive from +x and send to -x
		MPI_Sendrecv(&foxes[1],
				1,
				type_column_int,
				rank_neighbors_mx,
				tag,
				&foxes[nx_loc-1],
				1,
				type_column_int,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&foxes[nx_loc-1], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&foxes[1], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[3]);

		// Receive from -x and send to +x
		MPI_Sendrecv(&rabbits[nx_loc-2],
				1,
				type_column_int,
				rank_neighbors_px,
				tag,
				&rabbits[0],
				1,
				type_column_int,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[0], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[4]);
		// MPI_Isend(&rabbits[nx_loc-2], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[5]);

		// Receive from +x and send to -x
		MPI_Sendrecv(&rabbits[1],
				1,
				type_column_int,
				rank_neighbors_mx,
				tag,
				&rabbits[nx_loc-1],
				1,
				type_column_int,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[nx_loc-1], 1, type_column_int, rank_neighbors_px, tag, cartesian_communicator, &request[6]);
		// MPI_Isend(&rabbits[1], 1, type_column_int, rank_neighbors_mx, tag, cartesian_communicator, &request[7]);

		// MPI_Waitall(8,request,MPI_STATUS_IGNORE);

		// Receive from -y and send to +y
		MPI_Sendrecv(&foxes[(ny_loc-2) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				tag,
				&foxes[0],
				1,
				type_row_int,
				rank_neighbors_my,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&foxes[0], 1, type_row_int, rank_neighbors_my, tag, cartesian_communicator, &request[0]);
		// MPI_Isend(&foxes[(ny_loc-2) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, cartesian_communicator, &request[1]);

		// Receive from +y and send to -y
		MPI_Sendrecv(&foxes[nx_loc],
				1,
				type_row_int,
				rank_neighbors_my,
				tag,
				&foxes[(ny_loc-1) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&foxes[(ny_loc-1) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, cartesian_communicator, &request[2]);
		// MPI_Isend(&foxes[1], 1, type_row_int, rank_neighbors_my, tag, cartesian_communicator, &request[3]);

		// Receive from -y and send to +y
		MPI_Sendrecv(&rabbits[(ny_loc-2) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				tag,
				&rabbits[0],
				1,
				type_row_int,
				rank_neighbors_my,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[0], 1, type_row_int, rank_neighbors_my, tag, cartesian_communicator, &request[4]);
		// MPI_Isend(&rabbits[(ny_loc-2) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, cartesian_communicator, &request[5]);

		// Receive from +y and send to -y
		MPI_Sendrecv(&rabbits[nx_loc],
				1,
				type_row_int,
				rank_neighbors_my,
				tag,
				&rabbits[(ny_loc-1) * nx_loc],
				1,
				type_row_int,
				rank_neighbors_py,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		// MPI_Irecv(&rabbits[(ny_loc-1) * nx_loc], 1, type_row_int, rank_neighbors_py, tag, cartesian_communicator, &request[6]);
		// MPI_Isend(&rabbits[1], 1, type_row_int, rank_neighbors_my, tag, cartesian_communicator, &request[7]);

		// MPI_Waitall(8,request,MPI_STATUS_IGNORE);

		timer_com += MPI_Wtime() - timer_temp;

		timer_temp = MPI_Wtime();
		
		// Reduction
		sum_loc = 0.;
		for (iy = 1 ; iy < ny_loc-1 ; iy++) {
			for (ix = 1 ; ix < nx_loc-1 ; ix++) {
				sum_loc += (double)(foxes[iy*nx_loc+ix]);
			}
		}

		timer_comp += MPI_Wtime() - timer_temp;

		// MPI reduction in the rank 0

		timer_temp = MPI_Wtime();

		MPI_Reduce(&sum_loc,   // valeur local à réduire
				&sum_foxes[iteration],         // valeur finale
				1,                   // Nombre d'élèments
				MPI_DOUBLE,          // Type de donnée échangé
				MPI_SUM,             // Type de réduction
				0,                   // Rang de destination de la réduction
				cartesian_communicator       // Communicateur
		);

		timer_global_com += MPI_Wtime() - timer_temp;

		// sum_loc = 0;
		// for (iy = 1 ; iy < ny_loc-1 ; iy++) {
		// 	for (ix = 1 ; ix < nx_loc-1 ; ix++) {
		// 		sum_loc += (double)(foxes[iy*nx_loc+ix]);
		// 	}
		// }
		// std::cerr << sum_loc << std::endl;

		// Print
		if (rank == 0  && iteration%print_period == 0) {
			std::cout << " - iteration: " << iteration
						<< " - plants: " << sum_plants_before_rabbits[iteration]
						<< " - rabbits: " << sum_rabbits_before_foxes[iteration]
						<< " - foxes: "   << sum_foxes[iteration]
						<< std::endl;
		}

		// Diagnostics output
		if (iteration%diag_period == 0) {

			double plants_bloc_list[ny*nx];
			int rabbits_bloc_list[ny*nx];
			int foxes_bloc_list[ny*nx];
			double plants_diag[ny*nx];
			int rabbits_diag[ny*nx];
			int foxes_diag[ny*nx];

			MPI_Gather(&plants[1],1,bloc_double,&plants_bloc_list[0],(nx_loc-2)*(ny_loc-2),MPI_DOUBLE,0,cartesian_communicator);
			MPI_Gather(&rabbits[1],1,bloc_int,&rabbits_bloc_list[0],(nx_loc-2)*(ny_loc-2),MPI_INT,0,cartesian_communicator);
			MPI_Gather(&foxes[1],1,bloc_int,&foxes_bloc_list[0],(nx_loc-2)*(ny_loc-2),MPI_INT,0,cartesian_communicator);

		    if (rank == 0) {

				int start_index;

				for (int irank=0 ; irank < number_of_ranks ; irank++) {
					MPI_Cart_coords( cartesian_communicator, irank, 2, &rank_coordinates[0]);
					for (int iy = 0 ; iy < (ny_loc-2) ; iy++) {
						start_index = ((rank_coordinates[0] * (ny_loc-2))+iy) * nx + rank_coordinates[1] * (nx_loc-2);
						for (int ix = 0 ; ix < (nx_loc-2) ; ix++) {
							plants_diag[start_index + ix]  = plants_bloc_list[irank*(ny_loc-2)*(nx_loc-2) + iy * (nx_loc-2) + ix];
							rabbits_diag[start_index + ix] = rabbits_bloc_list[irank*(ny_loc-2)*(nx_loc-2) + iy * (nx_loc-2) + ix];
							foxes_diag[start_index + ix]   = foxes_bloc_list[irank*(ny_loc-2)*(nx_loc-2) + iy * (nx_loc-2) + ix];
						}
					}
				}

				// // --------------------------
				// // Binary format
				// // --------------------------

				char file_name[128];
				sprintf (file_name, "diags/diag_%03d.bin", iteration);

				//
				// std::string file_name = "diags/diags_" + std::to_string(iteration) + ".bin";
				//
				std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

				if(!binary_file) {
					std::cerr << " Error while creating the file :" << file_name << std::endl;
				}

				binary_file.write((char *) &iteration, sizeof(int));
				binary_file.write((char *) &nx, sizeof(int));
				binary_file.write((char *) &ny, sizeof(int));

				for (i = 0 ; i < nx*ny ; i++) {
					binary_file.write((char *) &plants_diag[i], sizeof(double));
				}

				//binary_file.write((char *) &plants_diag[0], sizeof(double)*nx*ny);
				binary_file.write((char *) &rabbits_diag[0], sizeof(int)*nx*ny);
				binary_file.write((char *) &foxes_diag[0], sizeof(int)*nx*ny);
				// binary_file.write((char *) &plants[0], sizeof(double)*nx_loc*ny_loc);
				// binary_file.write((char *) &rabbits[0], sizeof(int)*nx_loc*ny_loc);
				// binary_file.write((char *) &foxes[0], sizeof(int)*nx_loc*ny_loc);

				binary_file.close();

				// --------------------------
				// Ascii format
				// --------------------------

				sprintf (file_name, "diags/diag_%03d.dat", iteration);

				std::ofstream myfile;
				myfile.open(file_name);

				myfile << iteration << std::endl;
				myfile << nx << std::endl;
				myfile << ny << std::endl;

				for (i=0 ; i < nx*ny ; i++) {
					myfile << plants_diag[i] << " ";
				}
				myfile << std::endl;
				for (i=0 ; i < nx*ny ; i++) {
					myfile << (double)(rabbits_diag[i]) << " ";
				}
				myfile << std::endl;
				for (i=0 ; i < nx*ny ; i++) {
					myfile << (double)(foxes_diag[i]) << " ";
				}
				myfile << std::endl;

				myfile.close();

			}

			// delete [] plants_bloc_list;
			// delete [] rabbits_bloc_list;
			// delete [] foxes_bloc_list;
			//
			// delete [] plants_diag;

		}

	} // end iteration

	timer_main_loop = MPI_Wtime() - timer_main_loop;

	// Output scalar diagnostics ___________________________________________________

	// char file_name[128];
	// sprintf (file_name, "diags/scalars.bin");
	//
	// std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);
	//
	// if(!binary_file) {
	// 	std::cerr << " Error while creating the file :" << file_name << std::endl;
	// }
	//
	// binary_file.write((char *) &iterations, sizeof(iterations));
	//
	// // Plants
	// binary_file.write((char *) &sum_plants_before_rabbits[0], sizeof(double)*iterations);
	// binary_file.write((char *) &sum_plants_after_rabbits[0], sizeof(double)*iterations);
	// binary_file.write((char *) &sum_rabbits_before_foxes[0], sizeof(double)*iterations);
	// binary_file.write((char *) &sum_foxes[0], sizeof(double)*iterations);
	//
	// binary_file.close();

	// Bilan des timers __________________________________________________________________

	double percentage;

	double timers_array[5];
	timers_array[0] = timer_init;
	timers_array[1] = timer_main_loop;
	timers_array[2] = timer_com;
	timers_array[3] = timer_global_com;
	timers_array[4] = timer_comp;

	double minimum_times[5];
	double average_times[5];
	double maximum_times[5];

	MPI_Reduce(&timers_array[0], &minimum_times[0], 5, MPI_DOUBLE, MPI_MIN, 0, cartesian_communicator);
    MPI_Reduce(&timers_array[0], &average_times[0], 5, MPI_DOUBLE, MPI_SUM, 0, cartesian_communicator);
    MPI_Reduce(&timers_array[0], &maximum_times[0], 5, MPI_DOUBLE, MPI_MAX, 0, cartesian_communicator);

        for (int i=0 ; i < 5 ; i++) {
	    average_times[i] = average_times[i] / number_of_ranks;
	}

	if (rank == 0) {
		std::cout << " ------------------------------------ "<< std::endl;
		std::cout << " TIMERS"<< std::endl;
		std::cout << " ------------------------------------ "<< std::endl;
		std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
		std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;

		percentage = average_times[0] / (average_times[1] + average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Initialization" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;

		percentage = average_times[1] / (average_times[1] + average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Main loop" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;

		percentage = average_times[4] / (average_times[1] + average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Computation" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[4] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[4] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[4] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;

		percentage = average_times[2] / (average_times[1] + average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Point-to-point com" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;

		percentage = average_times[3] / (average_times[1] + average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Global com" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;
		
	}

    // Fialize MPI _______________________________________________________________

	MPI_Type_free(&type_column_double);
	MPI_Type_free(&type_row_double);
	MPI_Type_free(&type_column_int);
	MPI_Type_free(&type_row_int);
	MPI_Type_free(&bloc_double);
	MPI_Type_free(&bloc_int);

	delete [] plants;
	delete [] new_plants;

	delete [] rabbits;
	delete [] new_rabbits;

	delete [] foxes;
	delete [] new_foxes;

    MPI_Finalize();

} // end main
