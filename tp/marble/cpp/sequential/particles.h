#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parameters.h"
#include "walls.h"
//#include "hdf5.h"

//! class Particles
class Particles
{
    public:
    
    //Particles constructor
    Particles( );
    Particles( unsigned int number);

    // Particles destructor
    ~Particles();
    
    // Initialization functions
    void initialization(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties);
    void initialization_test(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Equation of movement applied to particles
    void push(double time_step, struct DomainProperties domain_properties);
    
    // Applied the walls to the particles
    void walls(double time_step, Walls walls);
    
    // Perform the binary collisions
    unsigned int collisions(struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Multiple collison iterations
    unsigned int multipleCollisions(struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Write all type of diags
    void writeDiags(struct TimeProperties time_properties, struct DiagProperties diag_properties);
    
    // Output specifically the hdf5 files
    // void writeHDF5(unsigned int iteration);
    
    // Output specifically the vtk files
    void writeVTK(unsigned int iteration);
    
    // Return the total energy in the domain
    double totalEnergy();
    
    // Return the maximal particle velocity
    double maxVelocity();
    
    private:
        
    double radius;    // Particle radius
    
    // Positions
    std::vector <double> x;
    std::vector <double> y;
    std::vector <double> z;

    // Velocities
    std::vector <double> vx;
    std::vector <double> vy;
    std::vector <double> vz;
    
    std::vector <double> mass;
    
    // Compute the distance between the specified particle and  the given wall
    double distance(unsigned int ip, struct Wall wall);
    
    // Compute the square of the velocity norm
    inline double squareVelocity(unsigned int ip) {
        return vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip];
    };

};

#endif
