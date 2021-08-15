/* _______________________________________________

    HEADER containing all the global parameters
  ________________________________________________ */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <cmath>
#include <vector>
#include <iostream>
#include <mpi.h>

/* _______________________________________________ */
/* Parameters */

struct Parameters {
    
    // ___________________________________________
    // Parameters for the simulation time
    
    double final;                       // Temps total de simulation
    double step;                        // Temps entre chaque itération
    int final_iteration;       // Final number of iterations
    int iteration;             // Number of iterations
    double current;                     // Current time

    // ___________________________________________
    // Parameters for the domain properties
    
    // Domain boundaries
    
    double xmin, xmax;
    double ymin, ymax;
    double zmin, zmax;
    
    // Gravity level
    
    double gravity_x;
    double gravity_y;
    double gravity_z;
    
    // Damping by friction
    
    double air_damping;
    double wall_damping;
    
    // Decomposition
    
    int n_patches_x;
    int n_patches_y;
    int n_patches_z;
    int n_patches;
    
    std::vector<int> id_patch_x;
    std::vector<int> id_patch_y;
    std::vector<int> id_patch_z;
    
    // ___________________________________________
    // Parameters for the diags
    
    int output_period;     // period in term of iteration between each diag
    int print_period;      // period in term of iteration between each diag
    bool hdf5;
    bool vtk;
    bool binary;
    
    // ___________________________________________
    // Parameters for the particles
    
    double radius;          // Particle radius
    double mass_min;        // Minimum mass
    double mass_max;        // Maximum mass
    double vmin;            // Minimal velocity for init
    double vmax;            // Maximal velocity for init
    int number;    // Initial number of particles
    double damping;         // damping coefficient for inelastic collisions
    int collision;         // Flag to activate the collisions
    int overlap;         // Flag to activate the collisions

    // ___________________________________________
    // Parameters for mpi

    int number_of_ranks; // Number of MPI ranks
    int rank;            // Number of MPI ranks
    int ranks_per_direction[3]; // Nnumber of ranks for each direction : x, y, z
    int periodicity[3]; // Nnumber of ranks for each direction : x, y, z
    int reorganisation;
    MPI_Comm cartesian_communicator;
    int rank_coordinates[3];
    int * topology_map;
    int exchange;
};

/* _______________________________________________ */
/* Headers */

// Header for the function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct Parameters & params);

#endif
