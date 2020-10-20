/* ______________________________________________________________________________

  3D spherical particle collision simulator

  Méthode :
  
  This code simulates the movement of particles including gravity, friction and collisions.
  
  Conditions limites :
  
  This code uses reflective walls with possible damping.

 - https://www.plasmaphysics.org.uk/collision3d.htm
 - https://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
 - https://www.sjsu.edu/faculty/watkins/collision.htm
 - https://studiofreya.com/3d-math-and-physics/simple-sphere-sphere-collision-detection-and-collision-response/
 - https://sites.google.com/site/tipebillard/modelisation-des-frottements

! ______________________________________________________________________________ */

#include "particles.h"
#include "patch.h"
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
    struct TimeProperties      time_properties;                // Time properties
    struct DiagProperties      diag_properties;     // Diags properties
    struct ParticleProperties  particle_properties; // Diags properties
    
    // Default configuration __________________________________________________________________________
    
    domain_properties.n_patches_x = 3;
    domain_properties.n_patches_y = 3;
    domain_properties.n_patches_z = 3;
    domain_properties.n_patches   = domain_properties.n_patches_x*domain_properties.n_patches_y*domain_properties.n_patches_z;
    
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
    
    time_properties.final           = 1;
    time_properties.final_iteration = 100;
    
    // Particle properties for init
    particle_properties.vmin      = 1E-5;
    particle_properties.vmax      = 0.1;
    particle_properties.number    = 2048;
    particle_properties.radius    = 0.01;
    particle_properties.damping   = 0.01;
    particle_properties.mass_min  = 0.1;
    particle_properties.mass_max  = 0.5;
    particle_properties.collision = true;
    
    // diagnostic parameters
    diag_properties.output_period = 1;
    diag_properties.print_period  = 20;
    diag_properties.hdf5          = false;
    diag_properties.vtk           = true;
    
    // Creation of the diag folder
    system("mkdir -p diags");
    
    // Command line arguments _________________________________________________________________________
    
    commandLineArguments(argc, argv, time_properties, particle_properties, domain_properties,diag_properties);
    
    time_properties.step = time_properties.final / time_properties.final_iteration;
    
    // Timers initialization ___________________________________________________________________________
    
    Timers timers;
    timers.add("diags");
    timers.add("collisions");
    timers.add("pusher");
    timers.add("wall");
    timers.add("exchange");
    
    // Particle initilization _________________________________________________________________________
    
    timers.start("initialization");

    Particles particles( domain_properties );
    
    particles.initTopology(domain_properties);
    particles.initParticles(domain_properties, time_properties, particle_properties);
    
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
    std::cout << " Topology:" << std::endl;
    std::cout << "  - number of patches: " << domain_properties.n_patches << std::endl;

    
    std::cout << std::endl;
    std::cout << " Particles properties:" << std::endl;
    particles.getTotalParticleNumber(particle_number);
    std::cout << "  - requested number of particles: " << particle_properties.number << std::endl;
    std::cout << "  - real number of particles: " << particle_number << std::endl;
    std::cout << "  - particle radius: " << particle_properties.radius << std::endl;
    std::cout << "  - particle inelastic damping: " << particle_properties.damping << std::endl;
    std::cout << "  - particle mass min: " << particle_properties.mass_min << std::endl;
    std::cout << "  - particle mass max: " << particle_properties.mass_max << std::endl;
    std::cout << "  - particle speed min: " << particle_properties.vmin << std::endl;
    std::cout << "  - particle speed max: " << particle_properties.vmax << std::endl;
    std::cout << "  - collision: " << particle_properties.collision << std::endl;
    
    std::cout << std::endl;
    std::cout << " Time properties:" << std::endl;
    std::cout << "  - simulation time: " << time_properties.final << std::endl;
    std::cout << "  - time step: " << time_properties.step << std::endl;
    std::cout << "  - number of iterations: " << time_properties.final_iteration << std::endl;
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
    
    time_properties.iteration = 0;
    particles.writeDiags(time_properties, diag_properties);
    
    timers.stop("diags");
    
    // Time loop _____________________________________________________________
    
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " MAIN LOOP"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    
    timers.start("main loop");
    
    for (time_properties.iteration = 1 ; time_properties.iteration <= time_properties.final_iteration; time_properties.iteration++) {
        
        // Global variables initialized ____________
        
        collision_counter = 0;
        total_energy = 0;
        max_velocity = 0;
        particle_number = 0;
        
        // Particle movements __________________
        
        timers.start("pusher");
        
        particles.push(time_properties, domain_properties);
        
        timers.stop("pusher");
        
        // Collisions between particles __________________
        
        timers.start("collisions");
        
        //collision_counter = particles.collisions(time_properties, particle_properties);
        particles.multipleCollisions(collision_counter, time_properties, domain_properties, particle_properties);
        
        timers.stop("collisions");
        
        // Collision with the walls __________________
        
        timers.start("wall");
        
        particles.walls(time_properties, domain_properties, walls);
        
        timers.stop("wall");
        
        // Exchange __________________
        
        timers.start("exchange");
        
        particles.exchange(domain_properties);
        
        timers.stop("exchange");
        
        // Diagnostics __________________
        
        timers.start("diags");
        
        particles.writeDiags(time_properties, diag_properties);
        
        if (time_properties.iteration%diag_properties.print_period == 0) {
        
            particles.getTotalEnergy(total_energy);
        
            particles.getMaxVelocity(max_velocity);
            
            particles.getTotalParticleNumber(particle_number);
        
            std::cout << " Iteration: " << std::setw(5) << time_properties.iteration
                      << " - total particles: " << std::setw(10) << particle_number
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
