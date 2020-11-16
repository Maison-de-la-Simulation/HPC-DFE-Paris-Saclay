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
    Particles( struct Parameters params );

    // Particles destructor
    ~Particles();
    
    // Initialize the topology for each patch
    void initTopology(struct Parameters params);
    
    // Initialize the particles for each patch
    void initParticles(struct Parameters params);
    
    // Equation of movement applied to particles
    void push(struct Parameters params);
    
    // Applied the walls to the particles
    void walls(struct Parameters params, Walls walls);
    
    // Perform the binary collisions
    unsigned int collisions(struct Parameters params);
    
    // Multiple collison iterations
    void multipleCollisions(unsigned int & collision_counter, struct Parameters params);
    
    // Exchange particles between patches
    void exchange(struct Parameters params);
    
    // Return the total energy in the domain (all patches)
    void getTotalEnergy(double & total_energy);
    
    // Return the maximal particle velocity in the domain (all patches)
    void getMaxVelocity(double & max_velocity);
    
    // Return the total number of particles
    void getTotalParticleNumber(unsigned int & total);
    
    // Output specifically the vtk files
    void writeVTK(unsigned int iteration);
    
    // Output specifically the binary files
    void writeBinary(unsigned int iteration);
    
    // Write all type of diags
    void writeDiags(struct Parameters params);
    
    private:
    
    // Array containing all patches of the domain
    std::vector <Patch> patches;
    
    // Total number of patches
    unsigned int n_patches;
    
};

#endif
