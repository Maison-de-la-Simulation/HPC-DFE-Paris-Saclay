#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parameters.h"
#include "hdf5.h"
#include "walls.h"

//! Structure that represents a container of particles
//! Includes their properties
struct ParticleContainer {
    std::vector <double> x;
    std::vector <double> y;
    std::vector <double> z;

    std::vector <double> vx;
    std::vector <double> vy;
    std::vector <double> vz;
    
    std::vector <double> mass;
};

//! class Particles
class Patch
{
    
    friend class Particles;
    
    public:
    
    //Particles constructor
    Patch( );
    Patch( unsigned int number);

    // Particles destructor
    ~Patch();
    
    // Topology
    // This function initializes the patch topology :
    // - number of patches in each direction
    // - id and coordinates of all patches
    void initTopology(struct DomainProperties domain_properties, unsigned int id);
    
    // Initialization functions
    void initParticles(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties);
    void initParticlesTest(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Equation of movement applied to particles
    void push(struct TimeProperties time, struct DomainProperties domain_properties);
    
    // Applied the walls to the particles
    void walls(struct TimeProperties time_properties, Walls walls);
    
    // Perform the binary collisions
    unsigned int collisions(struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Multiple collison iterations
    unsigned int multipleCollisions(struct TimeProperties time, struct ParticleProperties particle_properties);
    
    // Determine particles to exchange
    void computeExchangeBuffers(struct DomainProperties domain_properties);
    
    // Delete the particles leaving particles marked by the mask vector
    void deleteLeavingParticles();
    
    // Each patch copies the neighbor buffer in their inner particle list
    void receivedParticlesFromNeighbors(std::vector<Patch> & patches);
    
    // Return the total energy in the domain
    double getTotalEnergy();
    
    // Return the maximal particle velocity
    double getMaxVelocity();
    
    // Return the number of particles
    unsigned int getParticleNumber();
    
    // Check that all particles are in the domain
    void checkParticlesInDomain(struct DomainProperties domain_properties);
    
    protected:
    
    std::vector <double> x;
    std::vector <double> y;
    std::vector <double> z;

    std::vector <double> vx;
    std::vector <double> vy;
    std::vector <double> vz;
    
    std::vector <double> mass;
    
    std::vector <bool> mask;
    
    private:
    
    // For Particle exchange between patches
    std::vector <struct ParticleContainer> exchange;
    
    // Patch index
    unsigned int id;
    
    // Patch index in each direction in the cartesian topology
    unsigned int id_x;
    unsigned int id_z;
    unsigned int id_y;
    
    // Patch length in each direction
    double patch_x_length;
    double patch_y_length;
    double patch_z_length;
    
    // Flag to rapidly get if the patch is at the boundary
    bool at_mx_boundary;
    bool at_my_boundary;
    bool at_mz_boundary;
    bool at_px_boundary;
    bool at_py_boundary;
    bool at_pz_boundary;
    
    // Neightbors
    std::vector <int> neighbor_indexes;
    
    // Local boundaries of the patch
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    
    double radius;    // Particle radius
    
    // Compute the distance between the specified particle and the given wall
    double distance(unsigned int ip, struct Wall wall);
    
    // Compute the square of the velocity norm
    inline double squareVelocity(unsigned int ip) {
        return vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip];
    };
    
    // Convert the 3D index (x, y, z) of a patch into a global 1D index
    void patchIndex3DTo1D(struct DomainProperties domain_properties, unsigned int & id, unsigned int id_x, unsigned int id_y, unsigned int id_z) {
        id = id_z *(domain_properties.n_patches_y*domain_properties.n_patches_x) + id_y * domain_properties.n_patches_y + id_x;
    }

    void patchIndex1DTo3D(struct DomainProperties domain_properties, unsigned int id, unsigned int & id_x, unsigned int & id_y, unsigned int & id_z) {
        id_z = (id / (domain_properties.n_patches_y*domain_properties.n_patches_x) );
        id_y = (id - id_z * (domain_properties.n_patches_y*domain_properties.n_patches_x)) / domain_properties.n_patches_y;
        id_x = (id - id_z * (domain_properties.n_patches_y*domain_properties.n_patches_x) - id_y * domain_properties.n_patches_x);
    }
    
    // Return the index of the requested neighbor
    // If the neighbor does not exist the function returns -1
    int getNeighborIndex(struct DomainProperties domain_properties, int x_shift, int y_shift, int z_shift);
    
    // Return in which patch the particle is located relatively to the current patch.
    // xmin : shift = -1
    // xmax : shift = 1
    // ...
    void getParticlePatchShift(unsigned int ip, int & x_shift, int & y_shift, int & z_shift) {
        if (x[ip] < xmin) {
            x_shift = -1;
        } else if (x[ip] >= xmax) {
            x_shift = 1;
        } else {
            x_shift = 0;
        }
        
        if (y[ip] < ymin) {
            y_shift = -1;
        } else if (y[ip] >= ymax) {
            y_shift = 1;
        } else {
            y_shift = 0;
        }
        
        if (z[ip] < zmin) {
            z_shift = -1;
        } else if (z[ip] >= zmax) {
            z_shift = 1;
        } else {
            z_shift = 0;
        }
        
    }

};

#endif
