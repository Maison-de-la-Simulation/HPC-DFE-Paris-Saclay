/* ______________________________________________________________________________

  3D spherical particle collision simulator

  Méthode :
  Conditions limites :

 - https://www.plasmaphysics.org.uk/collision3d.htm
 - https://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
 - https://www.sjsu.edu/faculty/watkins/collision.htm
 - https://studiofreya.com/3d-math-and-physics/simple-sphere-sphere-collision-detection-and-collision-response/
 - https://sites.google.com/site/tipebillard/modelisation-des-frottements

! ______________________________________________________________________________ */

#include <iostream>
#include <vector>
#include "particles.h"
#include "parameters.h"
#include "walls.h"
#include "timers.h"

// Header for the function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct TimeProperties & time_properties,
                                                   struct ParticleProperties & particle_properties,
                                                   struct DomainProperties & domain_properties,
                                                   struct DiagProperties & diag_properties);

int main( int argc, char *argv[] )
{
    
    // Parameters ____________________________________________________________________________________
    
    unsigned int particle_number;   // Number of particles in the simulation
    double       total_energy;      // Total energy at each timestep
    double       max_velocity;      // Maximal velocity at each timestep
    unsigned int collision_counter; // Counter for collisions per timestep
    
    struct DomainProperties    domain_properties;   // Domain properties
    struct TimeProperties      time;                // Time properties
    struct DiagProperties      diag_properties;     // Diags properties
    struct ParticleProperties  particle_properties; // Diags properties
    
    // Default configuration __________________________________________________________________________
    
    domain_properties.xmin = 0;
    domain_properties.ymin = 0;
    domain_properties.zmin = 0;
    
    domain_properties.xmax = 1;
    domain_properties.ymax = 1;
    domain_properties.zmax = 1;
    
    domain_properties.gravity_x = 0;
    domain_properties.gravity_y = 0;
    domain_properties.gravity_z = -0.05;
    
    domain_properties.air_damping = 0.01;
    domain_properties.wall_damping = 0.01;
    
    time.final           = 1;
    time.final_iteration = 100;
    
    // Particle properties for init
    particle_properties.vmin     = 1E-5;
    particle_properties.vmax     = 0.1;
    particle_properties.number   = 2048;
    particle_properties.radius   = 0.01;
    particle_properties.damping  = 0.01;
    particle_properties.mass_min = 0.1;
    particle_properties.mass_max = 0.5;
    
    // diagnostic parameters
    diag_properties.output_period = 1;
    diag_properties.print_period  = 20;
    
    // Creation of the diag folder
    system("mkdir -p diags");
    
    // Command line arguments _________________________________________________________________________
    
    commandLineArguments(argc, argv, time, particle_properties, domain_properties,diag_properties);
    
    time.step = time.final / time.final_iteration;
    
    // Timers initialization ___________________________________________________________________________
    
    Timers timers;
    timers.add("diags");
    timers.add("collisions");
    timers.add("pusher");
    timers.add("wall");
    
    // Particle initilization _________________________________________________________________________
    
    timers.start("initialization");
    
    Particles particles;
    
    particles.initialization(domain_properties, time, particle_properties);
    //particles.initialization_2(domain_properties, time, particle_properties);
    
    timers.stop("initialization");
    
    // Wall initialization ____________________________________________________________________________
    
    Walls walls;
    
    double point[3] = {0, 0, 0};
    double normal[3] = {0, 0, 0};
    
    // xmin
    point[0] = domain_properties.xmin;
    normal[0] = 1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // xmax
    point[0] = domain_properties.xmax;
    normal[0] = -1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // ymin
    point[0] = 0;
    point[1] = domain_properties.ymin;
    normal[0] = 0;
    normal[1] = 1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // ymax
    point[1] = domain_properties.ymax;
    normal[1] = -1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // zmin
    point[1] = 0;
    point[2] = domain_properties.zmin;
    normal[1] = 0;
    normal[2] = 1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // ymax
    point[2] = domain_properties.zmax;
    normal[2] = -1;
    walls.add(point, normal, domain_properties.wall_damping);
    
    // Terminal output summary ________________________________________________________________
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " PARTICLE SIMULATION"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << std::endl;
    std::cout << " Particles properties:" << std::endl;
    std::cout << "  - number of particles: " << particle_properties.number << std::endl;
    std::cout << "  - particle radius: " << particle_properties.radius << std::endl;
    std::cout << "  - particle inelastic damping: " << particle_properties.damping << std::endl;
    std::cout << "  - particle mass min: " << particle_properties.mass_min << std::endl;
    std::cout << "  - particle mass max: " << particle_properties.mass_max << std::endl;
    std::cout << "  - particle speed min: " << particle_properties.vmin << std::endl;
    std::cout << "  - particle speed max: " << particle_properties.vmax << std::endl;
    
    std::cout << std::endl;
    std::cout << " Time properties:" << std::endl;
    std::cout << "  - simulation time: " << time.final << std::endl;
    std::cout << "  - time step: " << time.step << std::endl;
    std::cout << "  - number of iterations: " << time.final_iteration << std::endl;
    std::cout << "  - maximal time step: " <<  particle_properties.radius / (2 * particle_properties.vmax ) << std::endl;
    
    std::cout << std::endl;
    std::cout << " Domain properties:" << std::endl;
    std::cout << "  - domain x size: " << domain_properties.xmin << " " << domain_properties.xmax << std::endl;
    std::cout << "  - domain y size: " << domain_properties.ymin << " " << domain_properties.ymax << std::endl;
    std::cout << "  - domain z size: " << domain_properties.zmin << " " << domain_properties.zmax << std::endl;
    std::cout << "  - gravity: " << domain_properties.gravity_x << " " << domain_properties.gravity_y << " " << domain_properties.gravity_z << std::endl;
    std::cout << "  - air friction damping: " << domain_properties.air_damping << std::endl;
    std::cout << "  - wall friction damping: " << domain_properties.wall_damping << std::endl;
    
    std::cout << std::endl;
    std::cout << " Diag properties:" << std::endl;
    std::cout << "  - output period: " << diag_properties.output_period << std::endl;
    std::cout << "  - print period: " << diag_properties.print_period << std::endl;
    
    std::cout << std::endl;
    std::cout << " List of walls: " << std::endl;
    for(unsigned int iw = 0 ; iw < walls.size() ; iw++) {
        std::cout << " - Wall " << iw << std::endl;
        std::cout << "   * normal: " << walls(iw)->normal[0] << " " << walls(iw)->normal[1] << " " << walls(iw)->normal[2] << std::endl;
        std::cout << "   * d: " << walls(iw)->d << std::endl;
        std::cout << "   * damping coefficient: " << walls(iw)->damping << std::endl;
    }
    std::cout << std::endl;
    
    timers.start("diags");
    
    particles.writeDiags(0, diag_properties.output_period, true, true);
    
    timers.stop("diags");
    
    // Time loop _____________________________________________________________
    
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " MAIN LOOP"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    
    timers.start("main loop");
    
    for (time.iteration = 1 ; time.iteration <= time.final_iteration; time.iteration++) {
        
        collision_counter = 0;
        
        // Particle movements __________________
        
        timers.start("pusher");
        
        particles.push(time.step, domain_properties);
        
        timers.stop("pusher");
        
        // Collisions between particles __________________
        
        timers.start("collisions");
        
        //collision_counter = particles.collisions(time, particle_properties);
        collision_counter = particles.multipleCollisions(time, particle_properties);
        
        timers.stop("collisions");
        
        // Collision with the walls __________________
        
        timers.start("wall");
        
        particles.walls(time.step, walls);
        
        timers.stop("wall");
        
        // Diagnostics __________________
        
        timers.start("diags");
        
        particles.writeDiags(time.iteration, diag_properties.output_period, true, true);
        
        if (time.iteration%diag_properties.print_period == 0) {
            
            total_energy = particles.totalEnergy();
            
            max_velocity = particles.maxVelocity();
            
            std::cout << " Iteration: " << std::setw(5) << time.iteration
                      << " - total energy: " << std::setw(10) << total_energy
                      << " - collisions: " << std::setw(3) << collision_counter
                      << " - max v: " << std::setw(3) << max_velocity
                      << std::endl;
        }
        
        timers.stop("diags");
        
    }
    
    timers.stop("main loop");
    
    // Timers summary ____________________________________________________________
    
    std::cout << std::endl;
    
    timers.print();
    
}

// Function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct TimeProperties & time_properties,
                                                   struct ParticleProperties & particle_properties,
                                                   struct DomainProperties & domain_properties,
                                                   struct DiagProperties & diag_properties) {
    if (argc > 1) {
        unsigned int iarg = 1;
        std::string key;
        while(iarg < argc) {
            key = argv[iarg];
            if (key == "-t") {
                time_properties.final = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-it") {
                time_properties.final_iteration = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-np") {
                particle_properties.number = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-r") {
                particle_properties.radius = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-collision_damping") {
                particle_properties.damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-wall_damping") {
                domain_properties.wall_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-air_damping") {
                domain_properties.air_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-ve") {
                particle_properties.vmin = atoi(argv[iarg+1]);
                particle_properties.vmax = atoi(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-mass") {
                particle_properties.mass_min = atof(argv[iarg+1]);
                particle_properties.mass_max = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-gravity") {
                domain_properties.gravity_x = atof(argv[iarg+1]);
                domain_properties.gravity_y = atof(argv[iarg+2]);
                domain_properties.gravity_z = atof(argv[iarg+3]);
                iarg+=3;
            } else if (key == "-print") {
                diag_properties.print_period = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-diags") {
                diag_properties.output_period = atoi(argv[iarg+1]);
                iarg++;
            } else {
                iarg++;
            }
        }
    }
}
