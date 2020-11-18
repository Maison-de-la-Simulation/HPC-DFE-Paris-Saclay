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
    Patch( int number);

    // Particles destructor
    ~Patch();
    
    // Topology
    // This function initializes the patch topology :
    // - number of patches in each direction
    // - id and coordinates of all patches
    void initTopology(struct Parameters params, int id);
    
    // Initialization functions
    void initParticles(struct Parameters params);
    void initParticlesTest(struct Parameters params);
    
    // Equation of movement applied to particles
    void push(struct Parameters params);
    
    // Applied the walls to the particles
    void walls(struct Parameters params, Walls walls);
    
    // Perform the binary collisions
    int collisions(struct Parameters params);
    
    // Multiple collison iterations
    int multipleCollisions(struct Parameters params);
    
    // Determine particles to exchange
    void computeExchangeBuffers(struct Parameters params);
    
    // Delete the particles leaving particles marked by the mask vector
    void deleteLeavingParticles();
    
    // Each patch copies the neighbor buffer in their inner particle list
    void receivedParticlesFromNeighbors(std::vector<Patch> & patches);
    
    // Return the total energy in the domain
    double getTotalEnergy();
    
    // Return the maximal particle velocity
    double getMaxVelocity();
    
    // Return the number of particles
    int getParticleNumber();
    
    // Check that all particles are in the domain
    void checkParticlesInDomain(struct Parameters params);
    
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
    int id;
    
    // Patch index in each direction in the cartesian topology
    int id_x;
    int id_z;
    int id_y;
    
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
    double distance(int ip, struct Wall wall);
    
    // Compute the square of the velocity norm
    inline double squareVelocity(int ip) {
        return vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip];
    };
    
    // Convert the 3D index (x, y, z) of a patch into a global 1D index
    void patchCoordinatesToIndex(struct Parameters params, int & id, int id_x, int id_y, int id_z) {
        id = id_z *(params.n_patches_y*params.n_patches_x) + id_y * params.n_patches_y + id_x;
    }

    // This fonction gives the coordinates of the patch in the topology from the 1d index
    void patchIndexToCoordinates(struct Parameters params, int id, int & id_x, int & id_y, int & id_z) {
        id_z = (id / (params.n_patches_y*params.n_patches_x) );
        id_y = (id - id_z * (params.n_patches_y*params.n_patches_x)) / params.n_patches_y;
        id_x = (id - id_z * (params.n_patches_y*params.n_patches_x) - id_y * params.n_patches_x);
    }
    
    // Return the index of the requested neighbor
    // If the neighbor does not exist the function returns -1
    int getNeighborIndex(struct Parameters params, int x_shift, int y_shift, int z_shift);
    
    // Return in which patch the particle is located relatively to the current patch.
    // xmin : shift = -1
    // xmax : shift = 1
    // ...
    void getParticlePatchShift(int ip, int & x_shift, int & y_shift, int & z_shift);

};

#endif
