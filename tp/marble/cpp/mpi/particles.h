#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "parameters.h"
//#include "hdf5.h"
#include "walls.h"
#include "patch.h"


//! class Particles
class Particles
{
    
    public:
    
    //Particles constructor
    Particles( struct Parameters params);

    // Particles destructor
    ~Particles();
    
    // Initialize the topology for each patch
    void initTopology(struct Parameters & params);
    
    // Initialize the particles for each patch
    void initParticles(struct Parameters params);
    
    // Equation of movement applied to particles
    void push(struct Parameters params);
    
    // Applied the walls to the particles
    void walls(struct Parameters params, Walls walls);
    
    // Perform the binary collisions
    int collisions(struct Parameters params);
    
    // Multiple collison iterations
    void multipleCollisions(struct Parameters params);
    
    // Exchange particles between patches
    void exchange(struct Parameters params);
    
    // Return the total energy in the domain (all patches)
    void getTotalEnergy(struct Parameters params, double & total_energy);
    
    // Return the maximal particle velocity in the domain (all patches)
    void getMaxVelocity(struct Parameters params, double & max_velocity);
    
    // Return the total number of particles
    void getTotalParticleNumber(struct Parameters params, int & total, int & imbalance);
    
    // Return the total number of collisions
    void getTotalCollisionNumber(struct Parameters params, int & total);
    
    // Return the total number of exchange
    void getTotalExchangeNumber(struct Parameters params, int & total);
    
    // Output specifically the vtk files
    void writeVTK(int iteration, int number_of_particles, double * x, double * y, double * z,
    double * vx, double * vy, double * vz, double * mass);
    
    // Output specifically the binary files
    void writeBinary(int iteration, int number_of_particles, double * x, double * y, double * z,
    double * vx, double * vy, double * vz, double * mass);
    
    // Write all type of diags
    void writeDiags(struct Parameters params);
    
    private:
    
    // Array containing all patches of the domain
    std::vector <Patch> patches;
    
    // Total number of patches
    int n_patches;
    
};

#endif
