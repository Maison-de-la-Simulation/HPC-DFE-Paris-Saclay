#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parameters.h"
#include "hdf5.h"
#include "walls.h"
#include "patch.h"

//! class Particles
class Particles
{
    
    public:
    
    //Particles constructor
    Particles( struct DomainProperties domain_properties );

    // Particles destructor
    ~Particles();
    
    // Topology
    void initTopology(struct DomainProperties domain_properties);
    
    // Initialization
    void initParticles(struct DomainProperties domain_properties,
                        struct TimeProperties time_properties,
                        struct ParticleProperties particle_properties);
    
    // Equation of movement applied to particles
    void push(struct TimeProperties time, struct DomainProperties domain_properties);
    
    // Applied the walls to the particles
    void walls(struct TimeProperties time_properties, struct DomainProperties domain_properties, Walls walls);
    
    // Perform the binary collisions
    unsigned int collisions(struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Multiple collison iterations
    void multipleCollisions(unsigned int & collision_counter, struct TimeProperties time, struct DomainProperties domain_properties, struct ParticleProperties particle_properties);
    
    // Exchange particles between patches
    void exchange(struct DomainProperties domain_properties);
    
    // Return the total energy in the domain (all patches)
    void getTotalEnergy(double & total_energy);
    
    // Return the maximal particle velocity in the domain (all patches)
    void getMaxVelocity(double & max_velocity);
    
    // Return the total number of particles
    void getTotalParticleNumber(unsigned int & total);
    
    // Output specifically the vtk files
    void writeVTK(unsigned int iteration);
    
    // Write all type of diags
    void writeDiags(struct TimeProperties time_properties, struct DiagProperties diag_properties);
    
    private:
    
    std::vector <Patch> patches;
    
    unsigned int n_patches;
    
};

#endif
