/* ______________________________________________________________________________

  Maxwell solver (Yee FDTD)

  Sequential code
  Master DFE PARIS SACLAY
  Module HPC

! ______________________________________________________________________________ */

#include <cmath>
#include <iostream>
//#include <time.h>
#include <sys/time.h>
#include <iomanip>
#include <fstream>
#include <mpi.h>
#include <stdlib.h>

// Headers

void output_grids(int iteration,
                  int nx, int ny,
                  double * array,
                  std::string  field_name);

void gather_field(int size,
            int start,
            int rank,
            int number_of_ranks,
            double * field,
            double * field_global,
            MPI_Comm communicator);

// Main

int main( int argc, char *argv[] )
{
    // --------------------------------------------------------------------------
    // MPI initilization 
    // --------------------------------------------------------------------------

    // Initilization
    MPI_Init( NULL, NULL );

    int number_of_ranks;

    // We get the number of ranks
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_ranks );

    int rank;

    // We get the rank of the current MPI process
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    // --------------------------------------------------------------------------
    // Main parameters
    // --------------------------------------------------------------------------

    int nx;           // number of cells in the x direction without boundaries
    int ny;           // number of cells in the y direction without boundaries
    int iterations;   // number of iterations
    int diag_period;  // diagnostic output period
    int print_period; // print period
    double Lx;        // domain length
    double Ly;        // domain length

    int ranks_per_direction = 4;
    int periodicity = 0;
    int reorganisation = 1;
    MPI_Comm cartesian_communicator;

	int rank_neighbors_mx;
	int rank_neighbors_px;

	MPI_Request request[8];
	int rank_coordinates;
	int tag_send = 0;
    int tag_recv = 0;
    int tag = 0;

    // --------------------------------------------------------------------------
    // Default parameter initilization
    // --------------------------------------------------------------------------

    nx           = 128;
    ny           = 256;
    Lx           = 1;
    Ly           = 1;
    iterations   = 2000;
    diag_period  = 100;
    print_period = 100;
    
    // Antenna properties
    const double antenna_period = 1.2*Lx;
    const double antenna_length = 0.2*Lx;
    const double antenna_charge = 0.01;
    const double antenna_radius = 0.02*Lx;

    // --------------------------------------------------------------------------
    // Command line arguments
    // --------------------------------------------------------------------------

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

    // --------------------------------------------------------------------------
	// Creation of the cartesian topology
    // --------------------------------------------------------------------------

	MPI_Cart_create(MPI_COMM_WORLD,
                         1,
                         &number_of_ranks,
                         &periodicity,
                         reorganisation,
                         &cartesian_communicator);

	MPI_Comm_rank( cartesian_communicator, &rank );

	MPI_Cart_coords( cartesian_communicator, rank, 1, &rank_coordinates);

	// We get the neighbor ranks

	// En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)

	MPI_Cart_shift( cartesian_communicator, 0, 1, &rank_neighbors_mx, &rank_neighbors_px);

    // --------------------------------------------------------------------------
    // Initilization of arrays and internal parameters
    // --------------------------------------------------------------------------

    int nxd_global = nx;
    int nxp_global = nx-1;

    // Dual
    int nxd = nx / number_of_ranks;
    const int nyd = ny;

    if (rank_neighbors_mx >= 0) {
        nxd += 1;
    }

    if (rank_neighbors_px >= 0) {
        nxd += 1;
    }

    // Primal
    int nxp = nx / number_of_ranks - 1;
    int nyp = ny - 1;
    
    if (rank_neighbors_mx >= 0) {
        nxp += 1;
    }

    if (rank_neighbors_px >= 0) {
        nxp += 1;
    }

    // Electric field
    double * Ex = new double[nxd * nyp];
    double * Ey = new double[nxp * nyd];
    double * Ez = new double[nxp * nyp];
    
    // Magnetic field
    double * Bx = new double[nxp * nyd];
    double * By = new double[nxd * nyp];
    double * Bz = new double[nxd * nyd];

    // for diags
    double * Ex_global = new double[nxd_global*nyp];
    double * Ey_global = new double[nxp_global*nyd];
    double * Ez_global = new double[nxp_global*nyp];
    double * Bx_global = new double[nxp_global*nyd];
    double * By_global = new double[nxd_global*nyp];
    double * Bz_global = new double[nxd_global*nyd];

    // Space step
    const double dx = Lx / (nx-1);
    const double dy = Ly / (ny-1);

    // Time step (from the CFL)
    const double dt = 0.9/std::sqrt(1/(dx*dx) + 1/(dy*dy));
    
    // Antenna
    int izantenna = int(0.5*nyp) + int(0.5*nxp)*nyp;
    
    const double antenna_inverse_period = 1./antenna_period;
    const double antenna_max_velocity = 0.5*antenna_length*2*M_PI*antenna_inverse_period;

    // Iteration or timestep
    int iteration = 0;

    // Shortcuts
    const double dtdx = dt / dx;
    const double dtdy = dt / dy;

    double xmind;
    const double d = (nx / number_of_ranks)*dx ;
    xmind = d*rank_coordinates;
    if (rank_neighbors_mx >= 0 ) {
        xmind -= dx;
    }
    double xminp = xmind + 0.5*dx;

    // Initial field values
    for (int i = 0 ; i < nxp*nyp ; i++) {
        Ez[i] = 0;
    }
    for (int i = 0 ; i < nxp*nyd ; i++) {
        Ey[i] = 0;
        Bx[i] = 0;
    }
    for (int i = 0 ; i < nxd*nyp ; i++) {
        Ex[i] = 0;
        By[i] = 0;
    }
    for (int i = 0 ; i < nxd*nyd ; i++) {
        Bz[i] = 0;
    }
    
    // double xc = Lx*0.5;
    // double yc = Ly*0.5;
    // for (int iy = 0 ; iy < nyp ; iy++) {
    //     for (int ix = 0 ; ix < nxp ; ix++) {
    //         double x = ix*dx - xc;
    //         double y = iy*dy - yc;
    //         Ez[iy*nxp+ix] = std::exp(-(x*x + y*y) / 4.);
    //     }
    // }

    // MPI types

    // MPI_Datatype By_column;
    // MPI_Type_vector(nyp, 1, nxd, MPI_DOUBLE, &By_column);
    // MPI_Type_commit(&By_column);

    // MPI_Datatype Bz_column;
    // MPI_Type_vector(nyd, 1, nxd, MPI_DOUBLE, &Bz_column);
    // MPI_Type_commit(&Bz_column);

    double timer_com = 0;
    double timer_energy = 0;
    double timer_diag = 0;

    // Creation of the diag folder
    if (rank == 0) {
        system("mkdir -p diags");
    }

    // --------------------------------------------------------------------------
    // Summary in the terminal
    // --------------------------------------------------------------------------

    if (rank == 0) {
        std::cout << " ------------------------------------ "<< std::endl;
        std::cout << " MAXWELL SOLVER" << std::endl;
        std::cout << " ------------------------------------ "<< std::endl;

        std::cout << "  - nx (primal): " << nx << std::endl;
        std::cout << "  - ny (primal): " << ny << std::endl;
        std::cout << "  - dx: " << dx << std::endl;
        std::cout << "  - dy: " << dy << std::endl;
        std::cout << "  - iterations: " << iterations << std::endl;
        std::cout << "  - dt: " << dt << std::endl;
        std::cout << "  - duration: " << iterations*dt << std::endl;
        std::cout << "  - antenna period: " << antenna_period << std::endl;
        std::cout << "  - antenna max velocity: " << antenna_max_velocity << std::endl;
        std::cout << "  - print period: " << print_period << std::endl;

		std::cout << "  MPI properties: " << std::endl;
		std::cout << "  - number of ranks: " << number_of_ranks << std::endl;
		std::cout << "  - local number of cells in x (primal): " << nxp << std::endl;
		std::cout << "  - local number of cells in y (primal): " << nyp << std::endl;

        std::cout << std::endl;
    }

    // --------------------------------------------------------------------------
    // Affichage de la topologie
    // --------------------------------------------------------------------------

    // Affichage

    if (rank==0) std::cout <<  " Rank properties : "<< std::endl;

    for(int i=0 ; i < number_of_ranks ; i++) {
        if (rank == i) {
            std::cout << " Rank " << rank << " has topology coordinates ["  << rank_coordinates
                      << "] and neighbors: en -x " << rank_neighbors_mx 
                      << " and +x " << rank_neighbors_px
                      << ", xmin primal: " << xminp
                      << ", xmin dual: " << xmind
                      << ", nxp: " << nxp
                      << ", nyp: " << nyp
                      << ", nxd: " << nxd
                      << ", nyd: " << nyd
                      << std::endl;
        }
        MPI_Barrier(cartesian_communicator);
    }

    // --------------------------------------------------------------------------
    // Initial diagnostics output
    // --------------------------------------------------------------------------

    int size;
    int start;

    // _________________________________________
    // Ex (dual primal)

    if (rank == 0) {
        size = nxd * nyp;
        start = 0;
    } else if (rank_neighbors_px < 0) {
        size = nxd * nyp;
        start = nyp;
    } else {
        size = (nxd-1) * nyp;
        start = nyp;
    }

    gather_field(size,
                start,
                rank,
                number_of_ranks,
                Ex,
                Ex_global,
                cartesian_communicator);

    if (rank == 0) {

        output_grids(iteration,nxd_global,nyp,Ex_global,"Ex");
        output_grids(iteration,nxp_global,nyd,Ey_global,"Ey");
        output_grids(iteration,nxp_global,nyp,Ez_global,"Ez");

        output_grids(iteration,nxp,nyd,Bx,"Bx");
        output_grids(iteration,nxd,nyp,By,"By");
        output_grids(iteration,nxd,nyd,Bz,"Bz");
    }

    // --------------------------------------------------------------------------
    // Main loop
    // --------------------------------------------------------------------------

    if (rank == 0) {
        std::cout << " ------------------------------------------------------------------------- "<< std::endl;
        std::cout << " MAIN LOOP" << std::endl;
        std::cout << " ------------------------------------------------------------------------- "<< std::endl;
        std::cout << " Iteration |  time  | Ex energ | Ey energ | Ez energ |"<< std::endl;
        std::cout << " ----------|--------|----------|----------|----------|"<< std::endl;
    }

    double timer_begin = MPI_Wtime();

    for (iteration = 1 ; iteration <= iterations ; iteration++) {

        // Solve Maxwell Ampere
        // ------------------------------------- 

        // Ex (dual, primal)
        for (int ix = 0 ; ix < nxd ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                Ex[ix*nyp+iy] +=
                              + dtdy * (Bz[ix*nyd + iy + 1] - Bz[ix*nyd+iy]);
            }
        }

        // Ey (primal, dual)
        for (int ix = 0 ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyd ; iy++) {
                Ey[ix*nyd+iy] +=
                              - dtdx * (Bz[(ix+1)*nyd + iy] - Bz[ix*nyd + iy]);
            }
        }
        
        // Ez (primal, primal)
        for (int ix = 0 ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                Ez[ix*nyp+iy] +=
                                + dtdx * (By[(ix+1)*nyp + iy] - By[ix*nyp + iy])
                                - dtdy * (Bx[ix*nyd+iy+1] - Bx[ix*nyd+iy]);
            }
        }

        // Add antenas
        // -------------------------------------

        const double antenna_velocity = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);

        // Ex
        for (int ix = 0 ; ix < nxd ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                const double x = xmind + ix * dx;
                const double y = iy * dy;
                double xa = 0.5*Lx - 0.5*antenna_length*std::cos(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                double ya = 0.5*Ly;
                double va = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                if ( ((x-xa)*(x-xa) + (y-ya)*(y-ya)) <= antenna_radius*antenna_radius) {
                    Ex[ix*nyp+iy] += dt*antenna_charge*va;
                }
            }
        }

        //Ey
        for (int ix = 0 ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyd ; iy++) {
                const double x = xminp + ix * dx;
                const double y = iy * dy - 0.5*dy;
                double xa = 0.5*Lx;
                double ya = 0.5*Ly - 0.5*antenna_length*std::cos(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                double va = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                if ( ((x-xa)*(x-xa) + (y-ya)*(y-ya)) <= antenna_radius*antenna_radius) {
                    Ey[ix*nyd+iy] += dt*antenna_charge*va;
                }
            }
        }

        // Ey[iyantenna] = -dt*J;
        for (int ix = 0 ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                const double x = xminp + ix * dx;
                const double y = iy * dy - 0.5*dy;
                double xa = 0.5*Lx;
                double ya = 0.5*Ly;
                if ( ((x-xa)*(x-xa) + (y-ya)*(y-ya)) <= antenna_radius*antenna_radius) {
                    Ez[ix*nyp+iy] = -dt*antenna_charge*antenna_velocity;
                }
            }
        }

        // Solve Maxwell Faraday
        // -------------------------------------

        // Bx (primal, dual)
        for (int ix = 0 ; ix < nxp ; ix++) {
            for (int iy = 1 ; iy < nyd-1 ; iy++) {
                Bx[ix*nyd+iy] +=
                              - dtdy * (Ez[ix*nyp + iy] - Ez[ix*nyp + iy - 1]);
            }
        }
        // By (dual, primal)
        for (int ix = 1 ; ix < nxd-1 ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                By[ix*nyp + iy] +=
                                + dtdx * (Ez[ix * nyp + iy] - Ez[(ix-1) * nyp + iy]);
            }
        }
        
        // Bz (dual, dual)
        for (int ix = 1 ; ix < nxd-1 ; ix++) {
            for (int iy = 1 ; iy < nyd-1 ; iy++) {
                Bz[ix*nyd+iy] +=
                              + dtdy * ( Ex[ix*nyp+iy] - Ex[ix*nyp+iy-1])
                              - dtdx * ( Ey[ix*nyd+iy] - Ey[(ix-1)*nyd+iy] );
            }
        }

        // Communications
        // -------------------------------------

        const double timer_begin_com = MPI_Wtime();

        // Receive from +x and send to -x
		MPI_Sendrecv(&By[nyp],
				nyp,
				MPI_DOUBLE,
				rank_neighbors_mx,
				tag_send,
				&By[(nxd-1)*nyp],
				nyp,
				MPI_DOUBLE,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		MPI_Sendrecv(&Bz[nyd],
				nyd,
				MPI_DOUBLE,
				rank_neighbors_mx,
				tag_send,
				&Bz[(nxd-1)*nyd],
				nyd,
				MPI_DOUBLE,
				rank_neighbors_px,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

        // Receive from -x and send to +x
		MPI_Sendrecv(&By[(nxd-2)*nyp],
				nyp,
				MPI_DOUBLE,
				rank_neighbors_px,
				tag_send,
				&By[0],
				nyp,
				MPI_DOUBLE,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

		MPI_Sendrecv(&Bz[(nxd-2)*nyd],
				nyd,
				MPI_DOUBLE,
				rank_neighbors_px,
				tag_send,
				&Bz[0],
				nyd,
				MPI_DOUBLE,
				rank_neighbors_mx,
				MPI_ANY_TAG,
				cartesian_communicator,
				MPI_STATUS_IGNORE);

        timer_com += MPI_Wtime() - timer_begin_com;

        // Apply boundary conditions (Reflective)
        // -------------------------------------

        // MPI domain at the -X boundary
        if (rank_neighbors_mx < 0) {

            // Bord -X (ix = 0)
            for (int iy = 0 ; iy < nyp ; iy++) {
                By[iy] = By[iy + nyp];
            }
            for (int iy = 1 ; iy < nyd-1 ; iy++) {
                Bz[iy] = Bz[iy + nyd];
            }

        }

        // MPI domain at the +X boundary
        if (rank_neighbors_px < 0) {

            // Bord +X (ix = nxd - 1)
            for (int iy = 0 ; iy < nyp ; iy++) {
                By[iy + (nxd-1)*nyp] = By[iy + (nxd-2)*nyp];
            }
            for (int iy = 1 ; iy < nyd-1 ; iy++) {
                Bz[iy+ (nxd-1)*nyd] = Bz[iy + (nxd-2)*nyd];
            }

        }

        // Bord -Y (iy = 0)
        for (int ix = 0 ; ix < nxp ; ix++) {
            Bx[ix*nyd] = Bx[1+ix*nyd];
        }
        for (int ix = 0 ; ix < nxd ; ix++) {
            Bz[ix*nyd] = Bz[1+ix*nyd];
        }
        // Bord +Y (iy = nyd-1)
        for (int ix = 0 ; ix < nxp ; ix++) {
            Bx[nyd-1 + ix*nyd] = Bx[nyd-2 + ix*nyd];
        }
        for (int ix = 0 ; ix < nxd ; ix++) {
            Bz[nyd-1 + ix*nyd] = Bz[nyd-2 + ix*nyd];
        }
        
        // Compute Energy
        // -------------------------------------

        double Ex_energy = 0;
        double Ey_energy = 0;
        double Ez_energy = 0;

        int istartd = 0;
        int iendd = nxd;

        if (rank_neighbors_mx > 0) {
            istartd = 1;
        }
        if (rank_neighbors_px > 0) {
            iendd = nxd - 1;
        }

        for (int ix = istartd ; ix < iendd ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                Ex_energy += Ex[ix*nyp + iy]*Ex[ix*nyp + iy];
            }
        }
        for (int ix = istartd ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyd ; iy++) {
                Ey_energy += Ey[ix*nyd + iy]*Ey[ix*nyd + iy];
            }
        }
        for (int ix = istartd ; ix < nxp ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                Ez_energy += Ez[ix*nyp + iy]*Ez[ix*nyp + iy];
            }
        }

        Ex_energy *= dx*dy;
        Ey_energy *= dx*dy;
        Ez_energy *= dx*dy;

        const double timer_begin_energy = MPI_Wtime();

        double Ex_energy_global = 0;
        MPI_Reduce(&Ex_energy, 
                   &Ex_energy_global, 
                   1, 
                   MPI_DOUBLE, 
                   MPI_SUM,
                   0, 
                   cartesian_communicator);

        double Ey_energy_global = 0;
        MPI_Reduce(&Ey_energy, 
                   &Ey_energy_global, 
                   1, MPI_DOUBLE, 
                   MPI_SUM, 0, 
                   cartesian_communicator);

        double Ez_energy_global = 0;
        MPI_Reduce(&Ez_energy, 
                   &Ez_energy_global, 
                   1, 
                   MPI_DOUBLE, 
                   MPI_SUM, 0, 
                   cartesian_communicator);

        timer_energy += MPI_Wtime() - timer_begin_energy;

        // Diagnostics output
        // -------------------------------------

        if (iteration%diag_period == 0) {

            const double timer_begin_diag = MPI_Wtime();

            int size;
            int start;

            // _________________________________________
            // Ex (dual primal)

            if (rank == 0) {
                size = nxd * nyp;
                start = 0;
            } else if (rank_neighbors_px < 0) {
                size = nxd * nyp;
                start = nyp;
            } else {
                size = (nxd-1) * nyp;
                start = nyp;
            }

            gather_field(size,
                        start,
                        rank,
                        number_of_ranks,
                        Ex,
                        Ex_global,
                        cartesian_communicator);

            // _________________________________________
            // Ey (primal dual)

            if (rank == 0) {
                size = nxp * nyd;
                start = 0;
            } else {
                start = nyd;
                size = (nxp-1) * nyd;
            }

            gather_field(size,
                        start,
                        rank,
                        number_of_ranks,
                        Ey,
                        Ey_global,
                        cartesian_communicator);

            // _________________________________________
            // Ez (primal primal)

            if (rank == 0) {
                size = nxp * nyp;
                start = 0;
            } else {
                start = nyp;
                size = (nxp-1) * nyp;
            }

            gather_field(size,
                        start,
                        rank,
                        number_of_ranks,
                        Ez,
                        Ez_global,
                        cartesian_communicator);

            if (rank == 0) {
                output_grids(iteration,nxd_global,nyp,Ex_global,"Ex");
                output_grids(iteration,nxp_global,nyd,Ey_global,"Ey");
                output_grids(iteration,nxd_global,nyp,Ez_global,"Ez");

                output_grids(iteration,nxp,nyd,Bx,"Bx");
                output_grids(iteration,nxd,nyp,By,"By");
                output_grids(iteration,nxd,nyd,Bz,"Bz");
            }

            timer_diag += MPI_Wtime() - timer_begin_diag ;

        }
        
        // Print
        // -------------------------------------
        
        if (iteration%print_period == 0 and rank == 0) {
            std::cout << " " << std::setw(9) << iteration ;
            std::cout << " | " << std::fixed << std::setprecision(4) << std::setw(6) << iteration*dt ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << Ex_energy_global ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << Ey_energy_global ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << Ez_energy_global ;
            std::cout << " | " << std::endl;
        }
        
    }

    double timer_end = MPI_Wtime();

    // --------------------------------------------------------
    // Timers 
    // --------------------------------------------------------

    double timers_array[4];
    timers_array[0] = timer_end - timer_begin;
    timers_array[1] = timer_com;
    timers_array[2] = timer_energy;
    timers_array[3] = timer_diag;

    double minimum_times[4];
    double average_times[4];
    double maximum_times[4];

    MPI_Reduce(&timers_array[0], &minimum_times[0], 4, MPI_DOUBLE, MPI_MIN, 0, cartesian_communicator);
    MPI_Reduce(&timers_array[0], &average_times[0], 4, MPI_DOUBLE, MPI_SUM, 0, cartesian_communicator);
    MPI_Reduce(&timers_array[0], &maximum_times[0], 4, MPI_DOUBLE, MPI_MAX, 0, cartesian_communicator);

    for (int i=0 ; i < 4 ; i++) {
	    average_times[i] = average_times[i] / number_of_ranks;
	}

    if (rank == 0) {

        double percentage;

        std::cout << " ------------------------------------------------------------------------- "<< std::endl;
        std::cout << " TIMERS"<< std::endl;
        std::cout << " ------------------------------------------------------------------------- "<< std::endl;
        std::cout << "            code part |   min (s)  | averag (s) |   max (s)  | percentage |"<< std::endl;
        std::cout << " ---------------------|------------|------------|------------|------------|"<< std::endl;

		percentage = average_times[0] / (average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Main loop" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[0] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;
		percentage = average_times[1] / (average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Communication" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[1] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;
		percentage = average_times[2] / (average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Energy" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[2] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;
		percentage = average_times[3] / (average_times[0]) * 100;
		std::cout << " " << std::setw(20) << "Diag" ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << minimum_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << average_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << maximum_times[3] ;
		std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
		std::cout << " | " ;
		std::cout << std::endl;

    }

    // --------------------------------------------------------
    // Nettoyage 
    // --------------------------------------------------------

    delete [] Ex;
    delete [] Ey;
    delete [] Ez;
    
    delete [] Bx;
    delete [] By;
    delete [] Bz;

    delete [] Ex_global;
    delete [] Ey_global;
    delete [] Ez_global;
    delete [] Bx_global;
    delete [] By_global;
    delete [] Bz_global;

    MPI_Finalize();

}

// Function that write the grids on disk
void output_grids(int iteration,
                  int nx, int ny,
                  double * array,
                  std::string field_name) {

    char file_name[128];
    snprintf (file_name, 128, "diags/diag_%2s_%05d.bin", field_name.c_str(), iteration);

    std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

    if(!binary_file) {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }

    binary_file.write((char *) &iteration, sizeof(int));
    binary_file.write((char *) &nx, sizeof(int));
    binary_file.write((char *) &ny, sizeof(int));
    binary_file.write((char *) &array[0], sizeof(double)*nx*ny);
    binary_file.close();
}

// Function that gathers the field
void gather_field(int size,
            int start,
            int rank,
            int number_of_ranks,
            double * field,
            double * field_global,
            MPI_Comm communicator) {

    int size_array[number_of_ranks];

    MPI_Gather(&size, 1, MPI_INT, &size_array[0], 1, MPI_INT, 0, communicator);

    int displacement[number_of_ranks];
        
    displacement[0] = 0;
    for (int i = 1 ; i < number_of_ranks ; i++) {
        displacement[i] = displacement[i-1] + size_array[i-1];
    }

    MPI_Gatherv(&field[start],
                size,
                MPI_DOUBLE,
                &field_global[0],
                &size_array[0],
                &displacement[0],
                MPI_DOUBLE,
                0, communicator);

}