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

int main( int argc, char *argv[] )
{
    
    // Parameters ____________________________________________________________________________________
    
    int particle_number = 0;   // Number of particles in the simulation
    double       total_energy;          // Total energy at each timestep
    double       max_velocity;          // Maximal velocity at each timestep
    int collision_counter;     // Counter for collisions per timestep
    
    struct Parameters    params;   // Global properties
    
    // Default configuration __________________________________________________________________________
    
    params.n_patches_x = 3;
    params.n_patches_y = 3;
    params.n_patches_z = 3;
    
    params.xmin = 0;
    params.ymin = 0;
    params.zmin = 0;
    
    params.xmax = 1;
    params.ymax = 1;
    params.zmax = 1;
    
    params.gravity_x = 0;
    params.gravity_y = 0;
    params.gravity_z = -0.05;
    
    params.air_damping = 0.01;
    params.wall_damping = 0.01;
    
    params.final           = 1;
    params.final_iteration = 100;
    
    // Particle properties for init
    params.vmin      = 1E-5;
    params.vmax      = 0.1;
    params.number    = 2048;
    params.radius    = 0.01;
    params.damping   = 0.01;
    params.mass_min  = 0.1;
    params.mass_max  = 0.5;
    params.collision = true;
    
    // diagnostic parameters
    params.output_period = 1;
    params.print_period  = 20;
    params.hdf5          = false;
    params.vtk           = true;
    params.binary        = true;
    
    // Creation of the diag folder
    system("mkdir -p diags");
    
    // Command line arguments _________________________________________________________________________
    
    commandLineArguments(argc, argv, params);
    
    params.n_patches   = params.n_patches_x*params.n_patches_y*params.n_patches_z;
    
    params.step = params.final / params.final_iteration;
    
    // Timers initialization ___________________________________________________________________________
    
    Timers timers;
    timers.add("diags");
    timers.add("collisions");
    timers.add("pusher");
    timers.add("wall");
    timers.add("exchange");
    
    // Particle initilization _________________________________________________________________________
    
    timers.start("initialization");

    Particles particles( params );
    
    particles.initTopology(params);
    particles.initParticles(params);
    
    timers.stop("initialization");
    
    // Wall initialization ____________________________________________________________________________
    
    Walls walls;
    
    double point[3] = {0, 0, 0};
    double normal[3] = {0, 0, 0};
    
    // xmin
    point[0] = params.xmin;
    normal[0] = 1;
    walls.add(point, normal, params.wall_damping);
    
    // xmax
    point[0] = params.xmax;
    normal[0] = -1;
    walls.add(point, normal, params.wall_damping);
    
    // ymin
    point[0] = 0;
    point[1] = params.ymin;
    normal[0] = 0;
    normal[1] = 1;
    walls.add(point, normal, params.wall_damping);
    
    // ymax
    point[1] = params.ymax;
    normal[1] = -1;
    walls.add(point, normal, params.wall_damping);
    
    // zmin
    point[1] = 0;
    point[2] = params.zmin;
    normal[1] = 0;
    normal[2] = 1;
    walls.add(point, normal, params.wall_damping);
    
    // ymax
    point[2] = params.zmax;
    normal[2] = -1;
    walls.add(point, normal, params.wall_damping);
    
    // Terminal output summary ________________________________________________________________
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " PARTICLE SIMULATION"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << std::endl;
    std::cout << " Topology:" << std::endl;
    std::cout << "  - number of patches: " << params.n_patches << std::endl;

    
    std::cout << std::endl;
    std::cout << " Particles properties:" << std::endl;
    particles.getTotalParticleNumber(params, particle_number);
    std::cout << "  - requested number of particles: " << params.number << std::endl;
    std::cout << "  - real number of particles: " << particle_number << std::endl;
    std::cout << "  - particle radius: " << params.radius << std::endl;
    std::cout << "  - particle inelastic damping: " << params.damping << std::endl;
    std::cout << "  - particle mass min: " << params.mass_min << std::endl;
    std::cout << "  - particle mass max: " << params.mass_max << std::endl;
    std::cout << "  - particle speed min: " << params.vmin << std::endl;
    std::cout << "  - particle speed max: " << params.vmax << std::endl;
    std::cout << "  - collision: " << params.collision << std::endl;
    
    std::cout << std::endl;
    std::cout << " Time properties:" << std::endl;
    std::cout << "  - simulation time: " << params.final << std::endl;
    std::cout << "  - time step: " << params.step << std::endl;
    std::cout << "  - number of iterations: " << params.final_iteration << std::endl;
    std::cout << "  - maximal time step: " <<  params.radius / (2 * params.vmax ) << std::endl;
    
    std::cout << std::endl;
    std::cout << " Domain properties:" << std::endl;
    std::cout << "  - domain x size: " << params.xmin << " " << params.xmax << std::endl;
    std::cout << "  - domain y size: " << params.ymin << " " << params.ymax << std::endl;
    std::cout << "  - domain z size: " << params.zmin << " " << params.zmax << std::endl;
    std::cout << "  - gravity: " << params.gravity_x << " " << params.gravity_y << " " << params.gravity_z << std::endl;
    std::cout << "  - air friction damping: " << params.air_damping << std::endl;
    std::cout << "  - wall friction damping: " << params.wall_damping << std::endl;
    
    std::cout << std::endl;
    std::cout << " Diag properties:" << std::endl;
    std::cout << "  - output period: " << params.output_period << std::endl;
    std::cout << "  - print period: " << params.print_period << std::endl;
    
    std::cout << std::endl;
    std::cout << " List of walls: " << std::endl;
    for(int iw = 0 ; iw < walls.size() ; iw++) {
        std::cout << " - Wall " << iw << std::endl;
        std::cout << "   * normal: " << walls(iw)->normal[0] << " " << walls(iw)->normal[1] << " " << walls(iw)->normal[2] << std::endl;
        std::cout << "   * d: " << walls(iw)->d << std::endl;
        std::cout << "   * damping coefficient: " << walls(iw)->damping << std::endl;
    }
    std::cout << std::endl;
    
    timers.start("diags");
    
    params.iteration = 0;
    particles.writeDiags(params);
    
    timers.stop("diags");
    
    // Time loop _____________________________________________________________
    
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " MAIN LOOP"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    
    timers.start("main loop");
    
    for (params.iteration = 1 ; params.iteration <= params.final_iteration; params.iteration++) {
        
        // Global variables initialized ____________
        
        collision_counter = 0;
        total_energy = 0;
        max_velocity = 0;
        particle_number = 0;
        
        // Particle movements __________________
        
        timers.start("pusher");
        
        particles.push(params);
        
        timers.stop("pusher");
        
        // Collisions between particles __________________
        
        timers.start("collisions");
        
        //collision_counter = particles.collisions(params);
        particles.multipleCollisions(collision_counter, params);
        
        timers.stop("collisions");
        
        // Collision with the walls __________________
        
        timers.start("wall");
        
        particles.walls(params, walls);
        
        timers.stop("wall");
        
        // Exchange __________________
        
        timers.start("exchange");
        
        particles.exchange(params);
        
        timers.stop("exchange");
        
        // Diagnostics __________________
        
        timers.start("diags");
        
        particles.writeDiags(params);
        
        if (params.iteration%params.print_period == 0) {
        
            particles.getTotalEnergy(params, total_energy);
        
            particles.getMaxVelocity(params, max_velocity);
            
            particles.getTotalParticleNumber(params, particle_number);
        
            std::cout << " Iteration: " << std::setw(5) << params.iteration
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
