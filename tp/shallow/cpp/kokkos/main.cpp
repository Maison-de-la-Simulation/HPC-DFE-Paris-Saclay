// ____________________________________________________________________
//
// SHALLOW WATER 1D
// This program simulates the conservative 1d shallow water equations
// using Kokkos
//
// Description of the 1d structured discretized domain:
//
//  size is the number of points
//  |
//  |    `size - 1` is therefore the number of cells
//  |    |
//  v    v
//
//  |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|

//  <--> dx is the cell length
//
//  < -------- `length`is the domain length ----------------------->
// ____________________________________________________________________


// ____________________________________________________________________
// includes

#include <Kokkos_Core.hpp>
#include <iomanip>
#include <fstream>

int main() {
    Kokkos::initialize();

    { // scope necessary to remove deallocation warnings

    // ________________________________________________________________
    // Input parameters

    // space
    double length = 5.;
    int    size = 10000;

    // time
    int iterations = 20000;

    // gravity
    double g = 9.8;

    // Terminal output period
    int print_period = 1000;

    // Disk output period
    int output_period = 100;

    // ________________________________________________________________
    // Initilisation

    double dx = length / (size-1);
    double invdx = 1 / dx;
    //x = np.linspace ( 0, length, size );

    double dt = 0.1 * dx;
    double duration = iterations * dt;

    // define generic view for host and devcie
    using device_array = Kokkos::View<double *>;
    using host_array   = device_array::HostMirror;

    device_array x("x", size);
    device_array h("h", size);
    device_array uh("uh", size);

    device_array hm("hm", size-1);
    device_array uhm("uhm", size-1);

    // We initialize on CPU

    host_array x_host = Kokkos::create_mirror_view(x);
    host_array h_host = Kokkos::create_mirror_view(h);
    host_array uh_host = Kokkos::create_mirror_view(uh);

    for (auto i = 0 ; i < size ; ++i) {
        x_host[i] = i * dx;
    }

    for (auto i = 0 ; i < size ; ++i) {
        if (x_host[i] < 0.4 * length) {
            h_host[i] = 2.;
        } else if (x_host[i]> 0.6 * length) {
            h_host[i] = 1.;
        } else {
            h_host[i] = 1 + 0.5*(1. + std::cos(  M_PI * (x_host[i] - 0.4 * length) / (0.2*length)));
        }
        uh_host[i] = 0;
    }

    // Boundary conditions
    h_host[0]      = h_host[1];
    h_host[size-1] = h_host[size-2];

    // Copy from host to device.
    Kokkos::deep_copy(x, x_host);
    Kokkos::deep_copy(h, h_host);
    Kokkos::deep_copy(uh, uh_host);

    // Maximal and sum height
    double max_height = h_host[0];
    double sum_height = 0;
#if defined(KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA)
        Kokkos::parallel_reduce(size, KOKKOS_LAMBDA(const int i, double& local_max) { 
            local_max = h[i] > local_max ? h[i] : local_max; 
        }, Kokkos::Max<double>(max_height));

        Kokkos::parallel_reduce(size-1, KOKKOS_LAMBDA(const int i, double& local_sum) { 
            local_sum += 0.5 * (h[i] + h[i+1]);
        }, sum_height);
#endif

    // On host
    // for (auto i = 1 ; i < size ; ++i) {
    //     max_height = h[i] > max_height ? h[i] : max_height; 
    //     sum_height += 0.5 * (h[i] + h[i-1]);
    // }

    // Average height
    double average_height = sum_height / (size-1);
    // Water quantity
    double water_quantity = sum_height  * dx ;

    // Creation of the diag folder
    system("mkdir -p diags");

    // ________________________________________________________________
    // Terminal summary

    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " SHALLOW WATER 1D" << std::endl;
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;

    std::cout << "  - length: " << length << std::endl;
    std::cout << "  - size: " << size << std::endl;
    std::cout << "  - dx: " << dx << std::endl;
    std::cout << "  - duration: " << duration << std::endl;
    std::cout << "  - iterations: " << iterations << std::endl;
    std::cout << "  - dt: " << dt << std::endl;
    std::cout << "  - print period: " << print_period << std::endl;
    std::cout << "  - Execution space: " << typeid(Kokkos::DefaultExecutionSpace).name() << std::endl;
    std::cout << "  - max height: " << max_height << std::endl;
    std::cout << "  - mean height: " << average_height << std::endl;
    std::cout << std::endl;


    // ________________________________________________________________
    // Time loop
    
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " MAIN LOOP" << std::endl;
    std::cout << " ------------------------------------------------------------------------- "<< std::endl;
    std::cout << " Iteration | max h    | mean h   | water    |"<< std::endl;
    std::cout << " ----------| ---------|----------|----------|"<< std::endl;

    for (auto it = 0 ; it < iterations ; ++it) {

#if defined(KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA)
        Kokkos::parallel_for(size-1, KOKKOS_LAMBDA(const int i) {
          
        // Compute height and uh at midpoint

        hm[i] = 0.5 * ( h[i] + h[i+1] ) - ( 0.5 * dt ) * ( uh[i+1] - uh[i] ) * invdx;

        uhm[i] = 0.5 * ( uh[i] + uh[i+1] )  
            - 0.5 * dt * ( uh[i+1] * uh[i+1] / h[i+1] + 0.5 * g * h[i+1] * h[i+1]
            - uh[i] * uh[i]  / h[i] - 0.5 * g * h[i] * h[i] ) * invdx;

        });
#endif

        // Advance the height and uh to the next time step

#if defined(KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA)
        Kokkos::parallel_for(Kokkos::RangePolicy<>(1, size-1), KOKKOS_LAMBDA(const int i) {

        h[i] = h[i] - dt * ( uhm[i] - uhm[i-1] ) * invdx;

        uh[i] = uh[i] - dt * ( uhm[i] * uhm[i]  / hm[i] + 0.5 * g * hm[i] * hm[i] - uhm[i-1] * uhm[i-1] / hm[i-1] - 0.5 * g * hm[i-1] * hm[i-1] ) * invdx;

        });
#endif


        //  Reflective boundary conditions

#if defined(KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA)
        Kokkos::parallel_for(1, KOKKOS_LAMBDA(const int i) {

            h[0] = h[1];
            h[size-1] = h[size-2];
            uh[0] = - uh[1];
            uh[size-1] = - uh[size-2];

        });
#endif

        // Terminal information
        if (it%print_period == 0) {

            Kokkos::fence();

            // Maximal and sum height
            double max_height = 0;
            double sum_height = 0;
#if defined(KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA)
            Kokkos::parallel_reduce(size, KOKKOS_LAMBDA(const int i, double& local_max) { 
                local_max = h[i] > local_max ? h[i] : local_max; 
            }, Kokkos::Max<double>(max_height));

            Kokkos::parallel_reduce(size-1, KOKKOS_LAMBDA(const int i, double& local_sum) { 
                local_sum += 0.5 * (h[i] + h[i+1]);
            }, sum_height);
#endif
            // Average height
            double average_height = sum_height / (size-1);
            // Water quantity
            double water_quantity = sum_height  * dx ;

            std::cout << " " << std::setw(9) << it ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << max_height ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << average_height ;
            std::cout << " | " << std::scientific << std::setprecision(2) << std::setw(8) << water_quantity ;
            std::cout << " | " << std::endl;
        }

        // File output
        if (it%output_period == 0) {

            Kokkos::fence();

            // bring back data on host
            Kokkos::deep_copy(h_host, h);
            Kokkos::deep_copy(uh_host, uh);

            char file_name[128];
            snprintf (file_name, 128, "diags/diag_%05d.bin", it);

            std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

            if(!binary_file) {
                std::cerr << " Error while creating the file :" << file_name << std::endl;
            }

            binary_file.write((char *) &it, sizeof(int));
            binary_file.write((char *) &length, sizeof(double));
            binary_file.write((char *) &size, sizeof(int));
            binary_file.write((char *) &h_host[0], sizeof(double)*size);
            binary_file.write((char *) &uh_host[0], sizeof(double)*size);
            binary_file.close();
        }
    }

    } // scope necessary to remove deallocation warnings

    Kokkos::finalize();
}

