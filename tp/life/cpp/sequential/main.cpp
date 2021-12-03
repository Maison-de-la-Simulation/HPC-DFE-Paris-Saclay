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

// function to check if the cell is located outside the domain
bool ouside_domaine(int ix, int iy, int nx, int ny) {
	if (ix < 1 || ix > nx-2 || iy < 1 || iy > ny-2 ) {
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

	double plant_growing_velocity;    // How much a plant grows per iteration
	double seed_success;              // probability to have a new plant on a cell
	double max_plants_per_cell;       // Maximal number of plants per cell per iteration

	int max_babies_per_rabbits;       // Maximum number of babies a rabbit can have per iteration
	double rabbit_migration_success;  // probability for a rabbit to change cell
	double rabbits_hunger;            // How much plant eat a rabbit per iteration

	double fox_migration_success;     // probability for a fox to change cell
	double hunting_success;           // probability for a fox to catch a rabbit
	int max_babies_per_fox;           // Maximum number of babies a fox can have per iteration
	int max_foxes_per_cell;           // Maximum number of foxes per cell
	double fox_sens_of_smell;         // Capability to track rabbits in neighboring cells
	double fox_non_sociability;       // How strong the fox needs their own territory


	// Default configuration __________________________________________________________________________

	nx = 96;
	ny = 84;
	iterations = 1000;
	diag_period = 10;
	print_period = 100;

	// plants properties
	seed_success = 0.0001;
	max_plants_per_cell = 10.;
	plant_growing_velocity = 0.1;

	// rabbits properties
	rabbits_hunger      = 0.5;
	max_babies_per_rabbits = 5;
	rabbit_migration_success = 0.4;

	// fox properties
	hunting_success       = 0.4;
	fox_sens_of_smell     = 2.;
	max_foxes_per_cell    = 5;
	fox_migration_success = 0.9;
	fox_non_sociability   = 0.1;
	max_babies_per_fox    = 3;

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
			} else {
				iarg++;
			}
		}
	}

	// Creation of the diag folder
	system("mkdir -p diags");

	// We add the boundaries
	nx += 2;
	ny += 2;

	// Arrays and additional parameters

	double * plants = new double[nx*ny];
	double * new_plants = new double[nx*ny];

	int * rabbits = new int[nx*ny];
	int * new_rabbits = new int[nx*ny];

	int * foxes = new int[ny*nx];
	int * new_foxes = new int[ny*nx];

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

	struct timeval current_time;
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

	// Terminal output summary _________________________________________________

	std::cout << " ------------------------------------ "<< std::endl;
	std::cout << " GAME OF LIFE"<< std::endl;
	std::cout << " ------------------------------------ "<< std::endl;

	std::cout << "  - nx: " << nx-2 << std::endl;
	std::cout << "  - ny: " << ny-2 << std::endl;
	std::cout << "  - iterations: " << iterations << std::endl;
	std::cout << std::endl;
	std::cout << "  Plants properties: " << std::endl;
	std::cout << "  - plant growth velocity: " << plant_growing_velocity << std::endl;
	std::cout << "  - probability to have a seed growing: " << seed_success << std::endl;
	std::cout << "  - max plants per cell: " << max_plants_per_cell << std::endl;
	std::cout << std::endl;
	std::cout << "  Rabbits properties: " << std::endl;
	std::cout << "  - rabbits hunger: " << rabbits_hunger << std::endl;
	std::cout << std::endl;
	std::cout << "  Fox properties: " << std::endl;
	std::cout << "  - fox sens of smell: " << rabbits_hunger << std::endl;

	std::cout << std::endl;

	// Initialization __________________________________________________________

	gettimeofday(&current_time, NULL);
	timer_init = (current_time.tv_sec + current_time.tv_usec*1e-6);

	// random generator
	srand (timer_init);
	//srand(0);

	for (i = 0 ; i < ny*nx ; i++) {
		plants[i] = 0.;
		new_plants[i] = 0.;
		rabbits[i] = 0;
		new_rabbits[i] = 0;
		foxes[i] = 0;
		new_foxes[i] = 0;
	}

	for (iy = 1 ; iy < ny-1 ; iy++) {
		for (ix = 1 ; ix < nx-1 ; ix++) {
			plants[iy*nx + ix] = 1.;
		}
	}

	int amplitude = 2;
	for (ir = 0 ; ir < 5 ; ir++) {
		shiftx = rand()%(2*amplitude+1)-amplitude;
		shifty = rand()%(2*amplitude+1)-amplitude;
		ix = rand()%(nx-2*(amplitude+1)) + amplitude + 1;
		iy = rand()%(ny-2*(amplitude+1)) + amplitude + 1;
		rabbits[iy*nx + ix] = rand()%5 + 5;
		foxes[(iy+shifty)*nx + (ix+shiftx)] = rand()%5 + 5;
	}

	//rabbits[int(0.5*ny)*nx + int(0.5*nx)] = 5;

	// for (ir = 0 ; ir < 5 ; ir++) {
	// 	ix = rand()%(nx-2) + 1;
	// 	iy = rand()%(ny-2) + 1;
	// 	rabbits[iy*nx + ix] = rand()%5 + 5;
	// }

	// Main loop _______________________________________________________________

    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " MAIN LOOP"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;

	gettimeofday(&current_time, NULL);
	timer_main_loop = current_time.tv_sec + current_time.tv_usec*1e-6;
	timer_init = timer_main_loop - timer_init;

	for (iteration = 0 ; iteration < iterations ; iteration++) {

		// Step 1 - plants grow and propagate

		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {

				// We check if the cell ix, iy has pants around or inside the cell

				bool has_plants_around = false;

				for (shifty = -1 ; shifty < 2 ; shifty++) {
					for (shiftx = -1 ; shiftx < 2 ; shiftx++) {
						if (plants[(iy+shifty)*nx + (ix+shiftx)] > 0) {
							has_plants_around = true;
						}
					}
				}

				if (has_plants_around) {

					// Plants propagation or growth

					new_plants[iy*nx + ix] = std::min(max_plants_per_cell, plants[iy*nx + ix] + plant_growing_velocity);
					//new_plants[iy*nx + ix] = plants[iy*nx + ix] + 1.;

				} else {

					// Seed growth

					random_number = rand()/(double)RAND_MAX;

					if (random_number < seed_success) {
						new_plants[iy*nx + ix] += plant_growing_velocity;
					}

				}

			}
		}

		// Update of plants
		for (i = 0 ; i < ny*nx ; i++) {
			plants[i] = new_plants[i];
		}

		// Reduction
		sum_plants_before_rabbits[iteration] = 0.;
		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {
				sum_plants_before_rabbits[iteration] += new_plants[iy*nx+ix];
			}
		}

		// Step 2 - Rabbits

		// Reinitialize new rabbits before
		for (i = 0 ; i < ny*nx ; i++) {
			new_rabbits[i] = 0;
		}

		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {

				for (ir = 0 ; ir < rabbits[iy*nx + ix] ; ir++) {

					if (new_plants[iy*nx + ix] >= 1.) {

						// if there is enough food, the rabbits eat and make babies at the same location

						new_plants[iy*nx + ix] -= rabbits_hunger;
						new_rabbits[iy*nx + ix] += 1 + rand()%(max_babies_per_rabbits+1);

					} else {

						// all the plant has been eaten
						new_plants[iy*nx + ix] = 0.;

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
									if (ouside_domaine(ix+shiftx,iy+shifty,nx,ny)) {
										score[icell] = 0.;
									} else {
										icell2 = (iy+shifty)*nx + shiftx+ix;
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

							new_rabbits[(iy+shifty_list[icell])*nx + (ix+shiftx_list[icell])] += 1;

							// -------------------------------------
							// Method 2 - Rabbits migrate randomly

							// 	do {
							// 		shiftx = rand()%3-1;
							// 		shifty = rand()%3-1;
							//
							// 	} while ( ouside_domaine(ix+shiftx,iy+shifty,nx,ny) || ((shiftx == 0) && (shifty == 0)) );
							//
							// 	new_rabbits[(iy+shifty)*nx + (ix+shiftx)] += 1;


						}

					} // end if
				}
			}
		}

		// Array update
		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {
				rabbits[iy*nx + ix] = new_rabbits[iy*nx + ix];
				plants[iy*nx + ix] = new_plants[iy*nx + ix];
			}
		}

		// Reductions
		sum_rabbits_before_foxes[iteration] = 0.;
		sum_plants_after_rabbits[iteration] = 0.;
		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {
				sum_rabbits_before_foxes[iteration] += (double)(new_rabbits[iy*nx + ix]);
				sum_plants_after_rabbits[iteration] += (double)(new_plants[iy*nx + ix]);
			}
		}

		// Step 3 - Foxes

		// Reinitialization of the new foxes
		for (i = 0 ; i < ny*nx ; i++) {
			new_foxes[i] = 0;
		}

		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {

				// for each fox in the cell ix,iy
				for (ifox = 0 ; ifox < foxes[iy*nx + ix] ; ifox++) {

					if (new_rabbits[iy*nx + ix] >= 1 && foxes[iy*nx + ix] <= max_foxes_per_cell) {

						// if there is enough food, the foxes hunt a rabbit
						// and make babies at the same location

						random_number = rand()/(double)RAND_MAX;

						if (random_number <= hunting_success) {

							new_rabbits[iy*nx + ix] -= 1;
							new_foxes[iy*nx + ix] += 1 + rand()%(max_babies_per_fox+1);

						} else {

							// the fox stays in the same cell
							new_foxes[iy*nx + ix] += 1;

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
									if (ouside_domaine(ix+shiftx,iy+shifty,nx,ny)) {
										score[icell] = 0.;
									} else {
										icell2 = (iy+shifty)*nx + shiftx+ix;
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

							new_foxes[(iy+shifty_list[icell])*nx + (ix+shiftx_list[icell])] += 1;

							// -----------------------------------------------------
							// Method 2
							// 	do {
							// 		shiftx = rand()%3-1;
							// 		shifty = rand()%3-1;
							// 	} while ( ouside_domaine(ix+shiftx,iy+shifty,nx,ny) || ((shiftx == 0) && (shifty == 0)) );
							// 	new_foxes[(iy+shifty)*nx + (ix+shiftx)] += 1;

						}

					} // end if
				} // end ifox
			} //end ix
		} //end iy

		// Array update
		for (iy = 1 ; iy < ny-1 ; iy++) {
			for (ix = 1 ; ix < nx-1 ; ix++) {
				rabbits[iy*nx + ix] = new_rabbits[iy*nx + ix];
				foxes[iy*nx + ix] = new_foxes[iy*nx + ix];
			}
		}

		// Reduction
		sum_foxes[iteration] = 0;
		for (i = 0 ; i < ny*nx ; i++) {
			sum_foxes[iteration] += (double)(foxes[i]);
		}

		// Print
		if (iteration%print_period == 0) {
			std::cout << " - iteration: " << iteration
						<< " - plants: " << sum_plants_before_rabbits[iteration]
						<< " - rabbits: " << sum_rabbits_before_foxes[iteration]
						<< " - foxes: "   << sum_foxes[iteration]
						<< std::endl;
		}

		// Diagnostics output
		if (iteration%diag_period == 0) {

			// --------------------------
			// Binary format
			// --------------------------

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

			// Plants
			binary_file.write((char *) &plants[0], sizeof(double)*nx*ny);
			binary_file.write((char *) &rabbits[0], sizeof(int)*nx*ny);
			binary_file.write((char *) &foxes[0], sizeof(int)*nx*ny);

			binary_file.close();

			// --------------------------
			// Ascii format
			// --------------------------

			// sprintf (file_name, "diags/diag_%03d.dat", iteration);
			//
			// std::ofstream myfile;
			// myfile.open(file_name);
			//
			// myfile << iteration << std::endl;
			// myfile << nx << std::endl;
			// myfile << ny << std::endl;
			//
			// for (i=0 ; i < nx*ny ; i++) {
			// 	myfile << plants[i] << " ";
			// }
			// myfile << std::endl;
			// for (i=0 ; i < nx*ny ; i++) {
			// 	myfile << (double)(rabbits[i]) << " ";
			// }
			// myfile << std::endl;
			// for (i=0 ; i < nx*ny ; i++) {
			// 	myfile << (double)(foxes[i]) << " ";
			// }
			// myfile << std::endl;
			//
			// myfile.close();

		}

	} // end iteration

	gettimeofday(&current_time, NULL);
	timer_main_loop = (current_time.tv_sec + current_time.tv_usec*1e-6) - timer_main_loop;

	// Output scalar diagnostics ___________________________________________________

	char file_name[128];
	sprintf (file_name, "diags/scalars.bin");

	std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

	if(!binary_file) {
		std::cerr << " Error while creating the file :" << file_name << std::endl;
	}

	binary_file.write((char *) &iterations, sizeof(iterations));

	// Plants
	binary_file.write((char *) &sum_plants_before_rabbits[0], sizeof(double)*iterations);
	binary_file.write((char *) &sum_plants_after_rabbits[0], sizeof(double)*iterations);
	binary_file.write((char *) &sum_rabbits_before_foxes[0], sizeof(double)*iterations);
	binary_file.write((char *) &sum_foxes[0], sizeof(double)*iterations);

	binary_file.close();

	// Summary of the timers ___________________________________________________

	double percentage;

	std::cout << " ------------------------------------ "<< std::endl;
	std::cout << " TIMERS"<< std::endl;
	std::cout << " ------------------------------------ "<< std::endl;
	std::cout << "            code part |  time (s)  | percentage |"<< std::endl;
	std::cout << " ---------------------|------------|----------- |"<< std::endl;

	percentage = timer_init / (timer_main_loop + timer_init) * 100;
	std::cout << " " << std::setw(20) << "Initialization" ;
	std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << timer_init ;
	std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
	std::cout << " | " ;
	std::cout << std::endl;

	percentage = timer_main_loop / (timer_main_loop + timer_init) * 100;
	std::cout << " " << std::setw(20) << "Main loop" ;
	std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << timer_main_loop ;
	std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
	std::cout << " | " ;
	std::cout << std::endl;

	delete [] plants;
	delete [] new_plants;

	delete [] rabbits;
	delete [] new_rabbits;

	delete [] foxes;
	delete [] new_foxes;

} // end main
