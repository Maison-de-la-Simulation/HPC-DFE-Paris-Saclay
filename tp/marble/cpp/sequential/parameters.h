/* _______________________________________________

    HEADER containing all the parameters
  ________________________________________________ */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <cmath>
#include <iostream>

/* _______________________________________________ */
/* Parameters */

// Parameters for the simulation time
struct TimeProperties {
    
    double final;                       // Temps total de simulation
    double step;                        // Temps entre chaque itération
    unsigned int final_iteration;       // Final number of iterations
    unsigned int iteration;             // Number of iterations
    double current;                     // Current time
};

// Parameters for the domain properties
struct DomainProperties {
    
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
    
};

// Parameters for the diags
struct DiagProperties {
    
    unsigned int output_period;     // period in term of iteration between each diag
    unsigned int print_period;      // period in term of iteration between each diag
    bool hdf5;
    bool vtk;
};

// Parameters for the particles
struct ParticleProperties {
    
    double radius;          // Particle radius
    double mass_min;        // Minimum mass
    double mass_max;        // Maximum mass
    double vmin;            // Minimal velocity for init
    double vmax;            // Maximal velocity for init
    unsigned int number;    // Initial number of particles
    double damping;         // damping coefficient for inelastic collisions
    bool collision;         // Flag to activate the collisions
};

/* _______________________________________________ */
/* Headers */

// Header for the function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct TimeProperties & time_properties,
                                                   struct ParticleProperties & particle_properties,
                                                   struct DomainProperties & domain_properties,
                                                   struct DiagProperties & diag_properties);

#endif
