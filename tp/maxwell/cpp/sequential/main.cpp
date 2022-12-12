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

    // Antenna properties
    double antenna_period = 0.6*Lx;
    const double antenna_length = antenna_period;
    const double antenna_charge = 0.01;

    // --------------------------------------------------------------------------
    // Default parameter initilization
    // --------------------------------------------------------------------------

    nx           = 64;
    ny           = 128;
    Lx           = 1;
    Ly           = 1;
    iterations   = 10;
    diag_period  = 1;
    print_period = 1;
    
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
    
    // Antenna
    int ixantenna = int(0.5*nyp) * nxd + int(0.5*nxd);
    int iyantenna = int(0.5*nyd) * nxp + int(0.5*nxp);
    int izantenna = int(0.5*nyp) * nxp + int(0.5*nxp);

    const double antenna_max_velocity = 0.2*antenna_length*2*M_PI/ antenna_period;
    const double antenna_inverse_period = 1./antenna_period;

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
    std::cout << std::endl;
    
    // Time management
    struct timeval current_time;
    
    // Shortcuts
    const double dtdx = dt / dx;
    const double dtdy = dt / dy;

    // --------------------------------------------------------------------------
    // Initial diagnostics output
    // --------------------------------------------------------------------------

    int iteration = 0;

    char file_name[128];
    snprintf (file_name, 128, "diags/diag_%05d.bin", iteration);

    std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

    if(!binary_file) {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }

    binary_file.write((char *) &iteration, sizeof(int));
    binary_file.write((char *) &nx, sizeof(int));
    binary_file.write((char *) &ny, sizeof(int));
    
    binary_file.write((char *) &Ex[0], sizeof(double)*nxd*nyp);
    binary_file.write((char *) &Ey[0], sizeof(double)*nxp*nyd);
    binary_file.write((char *) &Ez[0], sizeof(double)*nxp*nyp);

    binary_file.write((char *) &Bx[0], sizeof(double)*nxp*nyd);
    binary_file.write((char *) &By[0], sizeof(double)*nxd*nyp);
    binary_file.write((char *) &Bz[0], sizeof(double)*nxd*nyd);

    binary_file.close();

    // --------------------------------------------------------------------------
    // Main loop
    // --------------------------------------------------------------------------

    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " MAIN LOOP" << std::endl;
    std::cout << " ------------------------------------ "<< std::endl;

    gettimeofday(&current_time, NULL);
    double timer_begin = current_time.tv_sec + current_time.tv_usec*1e-6;

    for (iteration = 1 ; iteration <= iterations ; iteration++) {

        // Solve Maxwell Ampere
        // ------------------------------------- 

        // Ex (dual, primal)
        for (int iy = 0 ; iy < nyp ; iy++) {
            for (int ix = 0 ; ix < nxd ; ix++) {
                Ex[iy*nxd+ix] +=
                              + dtdy * (Bz[(iy+1)*nxd + ix] - Bz[iy*nxd+ix]);
            }
        }
        
        // Ey (primal, dual)
        for (int iy = 0 ; iy < nyd ; iy++) {
            for (int ix = 0 ; ix < nxp ; ix++) {
                Ey[iy*nxp+ix] +=
                              - dtdx * (Bz[iy*nxd+ix+1] - Bz[iy*nxd+ix]);
            }
        }
        
        // Ez (primal, primal)
        for (int iy = 0 ; iy < nyp ; iy++) {
            for (int ix = 0 ; ix < nxp ; ix++) {
                Ez[iy*nxp+ix] +=
                                + dtdx * (By[iy*nxd+ix+1] - By[iy*nxd+ix])
                                - dtdy * (Bx[(iy+1)*nxp+ix] - Bx[iy*nxp+ix]);
            }
        }

        // Add antenas
        // -------------------------------------

        const double antenna_velocity = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);

        // Ex
        for (int iy = 0 ; iy < nyp ; iy++) {
            for (int ix = 0 ; ix < nxd ; ix++) {
                const double x = ix * dx - 0.5*dx;
                const double y = iy * dy;
                double xa = 0.5*Lx - 0.2*antenna_length*std::cos(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                double ya = 0.5*Ly;
                double va = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                if ((x >= xa-2*dx and x <= xa + 2*dx) and (y >= ya-2*dy and y <= ya + 2*dy)) {
                    Ex[iy*nxd+ix] += dt*antenna_charge*va;
                }
            }
        }

        //Ey
        for (int iy = 0 ; iy < nyd ; iy++) {
            for (int ix = 0 ; ix < nxp ; ix++) {
                const double x = ix * dx;
                const double y = iy * dy - 0.5*dy;
                double xa = 0.5*Lx;
                double ya = 0.5*Ly - 0.2*antenna_length*std::cos(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                double va = antenna_max_velocity*std::sin(2.0 * M_PI * iteration * dt * antenna_inverse_period);
                if ((x >= xa-2*dx and x <= xa + 2*dx) and (y >= ya-2*dy and y <= ya + 2*dy)) {
                    Ey[iy*nxp+ix] += dt*antenna_charge*va;
                }
            }
        }

        // Ey[iyantenna] = -dt*J;
        Ez[izantenna] = -dt*antenna_charge*antenna_velocity;

        // Solve Maxwell Faraday
        // -------------------------------------

        // Bx (primal, dual)
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            for (int ix = 0 ; ix < nxp ; ix++) {
                Bx[iy*nxp+ix] +=
                              - dtdy * (Ez[iy*nxp + ix] - Ez[(iy-1)*nxp + ix]);
            }
        }
        // By (dual, primal)
        for (int iy = 0 ; iy < nyp ; iy++) {
            for (int ix = 1 ; ix < nxd-1 ; ix++) {
                By[iy*nxd + ix] +=
                                + dtdx * (Ez[iy * nxp + ix] - Ez[iy * nxp + ix - 1]);
            }
        }
        
        // Bz (dual, dual)
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            for (int ix = 1 ; ix < nxd-1 ; ix++) {
                Bz[iy*nxd+ix] +=
                              + dtdy * ( Ex[iy*nxd+ix] - Ex[(iy-1)*nxd+ix])
                              - dtdx * ( Ey[iy*nxp+ix] - Ey[iy*nxp+ix-1] );
            }
        }

        // Apply boundary conditions (Reflective)
        // -------------------------------------

        // Bord -X (ix = 0)
        for (int iy = 0 ; iy < nyp ; iy++) {
            By[iy*nxd] = By[iy*nxd + 1];
        }
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            Bz[iy*nxd] = Bz[iy*nxd + 1];
        }
        // Bord +X (ix = nxd - 1)
        for (int iy = 0 ; iy < nyp ; iy++) {
            By[iy*nxd+nxd-1] = By[iy*nxd + nxd-2];
        }
        for (int iy = 1 ; iy < nyd-1 ; iy++) {
            Bz[iy*nxd+nxd-1] = Bz[iy*nxd + nxd-2];
        }
        // Bord -Y (iy = 0)
        for (int ix = 0 ; ix < nxp ; ix++) {
            Bx[ix] = Bx[nxp+ix];
        }
        for (int ix = 0 ; ix < nxd ; ix++) {
            Bz[ix] = Bz[nxd+ix];
        }
        // Bord +Y (iy = nyd-1)
        for (int ix = 0 ; ix < nxp ; ix++) {
            Bx[(nyd-1)*nxp + ix] = Bx[(nyd-2)*nxp + ix];
        }
        for (int ix = 0 ; ix < nxd ; ix++) {
            Bz[(nyd-1)*nxd + ix] = Bz[(nyd-2)*nxd + ix];
        }
        
        // Diagnostics output
        // -------------------------------------

        if (iteration%diag_period == 0) {
            char file_name[128];
            snprintf (file_name, 128, "diags/diag_%05d.bin", iteration);

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
            
            binary_file.write((char *) &Ex[0], sizeof(double)*nxd*nyp);
            binary_file.write((char *) &Ey[0], sizeof(double)*nxp*nyd);
            binary_file.write((char *) &Ez[0], sizeof(double)*nxp*nyp);

            binary_file.write((char *) &Bx[0], sizeof(double)*nxp*nyd);
            binary_file.write((char *) &By[0], sizeof(double)*nxd*nyp);
            binary_file.write((char *) &Bz[0], sizeof(double)*nxd*nyd);

            binary_file.close();
        }
        
        // Print
        // -------------------------------------
        
        if (iteration%print_period == 0) {
                std::cout << " - iteration: " << iteration
                    << " t: " << iteration*dt
                    << " - antenna velocity: " << antenna_velocity
                    << std::endl;
        }
        
    }

    gettimeofday(&current_time, NULL);
    double timer_end = current_time.tv_sec + current_time.tv_usec*1e-6;

    // Timers __________________________________________________________________

    double timer_main_loop = timer_end - timer_begin;

    double percentage;

    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " TIMERS"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << "            code part |  time (s)  | percentage |"<< std::endl;
    std::cout << " ---------------------|------------|----------- |"<< std::endl;

    percentage = timer_main_loop / (timer_main_loop) * 100;
    std::cout << " " << std::setw(20) << "Main loop" ;
    std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << timer_main_loop ;
    std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
    std::cout << " | " ;
    std::cout << std::endl;

}
