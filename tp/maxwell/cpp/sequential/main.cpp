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

// Headers

void output_grids(int iteration,
                  int nx, int ny,
                  double * array,
                  std::string  field_name);

// Main

int main( int argc, char *argv[] )
{

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

    // --------------------------------------------------------------------------
    // Default parameter initilization
    // --------------------------------------------------------------------------

    nx           = 127;
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
    // Initilization of arrays and internal parameters
    // --------------------------------------------------------------------------

    // Primal
    const int nxp = nx;
    const int nyp = ny;
    
    // Dual
    const int nxd = nx + 1;
    const int nyd = ny + 1;

    // Electric field
    double * Ex = new double[nxd * nyp];
    double * Ey = new double[nxp * nyd];
    double * Ez = new double[nxp * nyp];
    
    // Magnetic field
    double * Bx = new double[nxp * nyd];
    double * By = new double[nxd * nyp];
    double * Bz = new double[nxd * nyd];

    // Space step
    const double dx = Lx / (nx-1);
    const double dy = Ly / (ny-1);
    
    // Time step (from the CFL)
    const double dt = 0.9/std::sqrt(1/(dx*dx) + 1/(dy*dy));

    // Iteration or timestep
    int iteration = 0;
    
    // Antenna
    int izantenna = int(0.5*nyp) + int(0.5*nxp)*nyp;

    const double antenna_inverse_period = 1./antenna_period;
    const double antenna_max_velocity = 0.5*antenna_length*2*M_PI*antenna_inverse_period;

    // Time management
    struct timeval current_time;
    
    // Shortcuts
    const double dtdx = dt / dx;
    const double dtdy = dt / dy;

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

    // Creation of the diag folder
    system("mkdir -p diags");
    
    // --------------------------------------------------------------------------
    // Summary in the terminal
    // --------------------------------------------------------------------------

    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " MAXWELL SOLVER" << std::endl;
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;

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
    std::cout << std::endl;

    // --------------------------------------------------------------------------
    // Initial diagnostics output
    // --------------------------------------------------------------------------

    output_grids(iteration,nxd,nyp,Ex,"Ex");
    output_grids(iteration,nxp,nyd,Ey,"Ey");
    output_grids(iteration,nxp,nyp,Ez,"Ez");

    output_grids(iteration,nxp,nyd,Bx,"Bx");
    output_grids(iteration,nxd,nyp,By,"By");
    output_grids(iteration,nxd,nyd,Bz,"Bz");

    // --------------------------------------------------------------------------
    // Main loop
    // --------------------------------------------------------------------------

    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " MAIN LOOP" << std::endl;
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " Iteration |  time  | Ex energ |"<< std::endl;
    std::cout << " ----------|--------|----------|"<< std::endl;

    gettimeofday(&current_time, NULL);
    double timer_begin = current_time.tv_sec + current_time.tv_usec*1e-6;

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
                const double x = ix * dx - 0.5*dx;
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
                const double x = ix * dx;
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
                const double x = ix * dx;
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

        // Apply boundary conditions (Reflective)
        // -------------------------------------

        // Bord -X (ix = 0)
        for (int iy = 0 ; iy < nyp ; iy++) {
            By[iy] = By[iy + nyp];
        }
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            Bz[iy] = Bz[iy + nyd];
        }
        // Bord +X (ix = nxd - 1)
        for (int iy = 0 ; iy < nyp ; iy++) {
            By[iy + (nxd-1)*nyp] = By[iy + (nxd-2)*nyp];
        }
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            Bz[iy+ (nxd-1)*nyd] = Bz[iy + (nxd-2)*nyd];
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
        for (int ix = 0 ; ix < nxd ; ix++) {
            for (int iy = 0 ; iy < nyp ; iy++) {
                Ex_energy += Ex[ix*nyp + iy]*Ex[ix*nyp + iy];
            }
        }


        // Diagnostics output
        // -------------------------------------

        if (iteration%diag_period == 0) {
            output_grids(iteration,nxd,nyp,Ex,"Ex");
            output_grids(iteration,nxp,nyd,Ey,"Ey");
            output_grids(iteration,nxp,nyp,Ez,"Ez");

            output_grids(iteration,nxp,nyd,Bx,"Bx");
            output_grids(iteration,nxd,nyp,By,"By");
            output_grids(iteration,nxd,nyd,Bz,"Bz");
        }
        
        // Print
        // -------------------------------------
        
        if (iteration%print_period == 0) {
            std::cout << " " << std::setw(9) << iteration ;
            std::cout << " | " << std::fixed << std::setprecision(4) << std::setw(6) << iteration*dt ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << Ex_energy ;
            std::cout << " | " << std::endl;
        }
        
    }

    gettimeofday(&current_time, NULL);
    double timer_end = current_time.tv_sec + current_time.tv_usec*1e-6;

    // --------------------------------------------------------
    // Timers 
    // --------------------------------------------------------

    double timer_main_loop = timer_end - timer_begin;

    double percentage;

    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " TIMERS"<< std::endl;
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << "            code part |  time (s)  | percentage |"<< std::endl;
    std::cout << " ---------------------|------------|----------- |"<< std::endl;

    percentage = timer_main_loop / (timer_main_loop) * 100;
    std::cout << " " << std::setw(20) << "Main loop" ;
    std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << timer_main_loop ;
    std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
    std::cout << " | " ;
    std::cout << std::endl;

    // --------------------------------------------------------
    // Cleaning 
    // --------------------------------------------------------

    delete [] Ex;
    delete [] Ey;
    delete [] Ez;
    
    delete [] Bx;
    delete [] By;
    delete [] Bz;

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
