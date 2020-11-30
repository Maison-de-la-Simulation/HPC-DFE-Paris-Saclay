#include "particles.h"

// Constructor for Particles
Particles::Particles(struct Parameters params)
{
    n_patches = params.n_patches;
    patches.resize(n_patches);
};

// Destructor
Particles::~Particles() {
    
};

// Initialize the topology for each patch
void Particles::initTopology(struct Parameters params) {
    
    if (params.n_patches != params.n_patches_x * params.n_patches_y * params.n_patches_z) {
        std::cerr << " CONFIGURATION ERROR: the total number of patches must match the topology." << std::endl;
        exit(0);
    }
    
    for (int i_patch = 0 ; i_patch < params.n_patches; i_patch++) {
        patches[i_patch].initTopology(params, i_patch);
    }
    
};

// Initialize the particles for each patch
void Particles::initParticles(struct Parameters params) {

    if (2*params.vmax * params.step > params.radius&&(params.collision > 0)) {
        std::cerr << " CONFIGURATION ERROR: a particle cannot cross more than a radius during a time step." << std::endl;
        std::cerr << " Maximal distanced: " << 2*params.vmax * params.step << " / radius: " << params.radius << std::endl;
        exit(0);
    }

    for (int i_patch = 0 ; i_patch < params.n_patches; i_patch++) {
       patches[i_patch].initParticles(params);
    }
    
};

// Push the particles using the velocity during the given time step
// This function solves the equations of movements
void Particles::push(struct Parameters params) {
    
    for (int i_patch = 0 ; i_patch < params.n_patches; i_patch++) {
       patches[i_patch].push(params);
    }

}

// Applied the walls to the particles
void Particles::walls(struct Parameters params, Walls walls) {
    // We perform the walls several times in case of multiple rebound
    for (int iw = 0 ; iw < 3; iw++) {
        for (int i_patch = 0 ; i_patch < params.n_patches; i_patch++) {
            patches[i_patch].walls(params, walls);
        }
    }
}

// Multiple collison iterations
void Particles::multipleCollisions(struct Parameters params) {
    if (params.collision) {
        
        for (int i_patch = 0 ; i_patch < params.n_patches; i_patch++) {
            patches[i_patch].multipleCollisions(params);
        }
        
    }
}

// Exchange particles between patches
void Particles::exchange(struct Parameters params) {
    
    // step 1 : we identify the particles that leave the current patch to another
    // We copy these particles in buffers.
    // There is a buffer per direction, i.e. 26 in 3D.
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
        patches[i_patch].computeExchangeBuffers(params);
    }
    
    // step 2: we delete the particles put in the buffers from the main array
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
        patches[i_patch].deleteLeavingParticles();
    }
    
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
        patches[i_patch].receivedParticlesFromNeighbors(patches);
    }
    
}

// Return the total energy
void Particles::getTotalEnergy(struct Parameters params, double & total_energy) {
    
    double patch_energy;
    
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
       patch_energy = patches[i_patch].getTotalEnergy();
       total_energy += patch_energy;
    }
    
}

// Return the maximal particle velocity
void Particles::getMaxVelocity(struct Parameters params, double & max_velocity) {
    
    double patch_max_velocity;

    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
       patch_max_velocity = patches[i_patch].getMaxVelocity();
       max_velocity = std::max(patch_max_velocity, max_velocity);
    }
}

// Return the total number of particles
void Particles::getTotalParticleNumber(struct Parameters params, int & total) {
    
    total = 0;
    
    int local;
    
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
       local = patches[i_patch].getParticleNumber();
       total += local;
    }
}

// Return the total number of collisions
void Particles::getTotalCollisionNumber(struct Parameters params, int & total) {
    
    total = 0;
    
    int local;
    
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
       local = patches[i_patch].getCollisionNumber();
       total += local;
    }
}

// Write the particle diagnostics
void Particles::writeDiags(struct Parameters params) {
    if (params.iteration%params.output_period == 0) {
        // if (params.hdf5) {
        //     writeHDF5(params.iteration);
        // }
        if (params.vtk) {
            writeVTK(params);
        }
        if (params.binary) {
            writeBinary(params);
        }
    }
}


// Write the particle properties in a vitk file
void Particles::writeVTK(struct Parameters params) {
    
    std::string file_name = "diags/particles_" + std::to_string(params.iteration) + ".vtk";
    
    std::ofstream vtk_file(file_name.c_str(), std::ios::out | std::ios::trunc);
    
    if (vtk_file)
    {
        vtk_file << "# vtk DataFile Version 3.0" << std::endl;
        vtk_file << "vtk output" << std::endl;
        vtk_file << "ASCII" << std::endl;
        vtk_file << "DATASET POLYDATA" << std::endl;
        
        int number = 0;
        getTotalParticleNumber(params, number);
        
        // Particle positions
        vtk_file << std::endl;
        vtk_file << "POINTS "<< number << " float" << std::endl;
        for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
            for(int ip = 0 ; ip < patches[i_patch].getParticleNumber() ; ip++) {
              vtk_file << patches[i_patch].x[ip] << " ";
              vtk_file << patches[i_patch].y[ip] << " " ;
              vtk_file << patches[i_patch].z[ip] << std::endl ;
            }
        }
        
        // Construction of the mass
        vtk_file << std::endl;
        vtk_file << "POINT_DATA " << number  << std::endl;
        vtk_file << "SCALARS mass float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
            for(int ip = 0 ; ip < patches[i_patch].getParticleNumber() ; ip++) {
                vtk_file << patches[i_patch].mass[ip] << " " ;
            }
        }
        vtk_file << std::endl;
        
        // Construction of the energy
        vtk_file << std::endl;
        vtk_file << "SCALARS energy float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
            for(int ip = 0 ; ip < patches[i_patch].getParticleNumber() ; ip++) {
                vtk_file << 0.5*patches[i_patch].mass[ip]*(patches[i_patch].vx[ip]*patches[i_patch].vx[ip] +
                                        patches[i_patch].vy[ip]*patches[i_patch].vy[ip] +
                                        patches[i_patch].vz[ip]*patches[i_patch].vz[ip]) << " " ;
            }
        }
        vtk_file << std::endl;
        
        // Construction of the radius
        vtk_file << std::endl;
        vtk_file << "SCALARS radius float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
            for(int ip = 0 ; ip < patches[i_patch].getParticleNumber() ; ip++) {
              vtk_file << patches[i_patch].radius << " " ;
            }
        }
        vtk_file << std::endl;
        
        // Construction of the speed vector
        vtk_file << std::endl;
        vtk_file << "VECTORS v float" << std::endl;
        for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
            for(int ip = 0 ; ip < patches[i_patch].getParticleNumber() ; ip++) {
                vtk_file << patches[i_patch].vx[ip] << " ";
                vtk_file << patches[i_patch].vy[ip] << " ";
                vtk_file << patches[i_patch].vz[ip] << " ";
            }
        }
        
    }
    else
    {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }
}

// Write the particle properties in a binary file
void Particles::writeBinary(struct Parameters params) {
    
    std::string file_name = "diags/particles_" + std::to_string(params.iteration) + ".bin";
    
    std::ofstream binary_file(file_name.c_str(), std::ios::out | std::ios::binary);
    
    if(!binary_file) {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }
    
    int number;
    getTotalParticleNumber(params,number);
    
    binary_file.write((char *) &number, sizeof(number));
    binary_file.write((char *) &patches[0].radius, sizeof(double));
    
    // Particle positions
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].x[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].y[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].z[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    
    // Particle velocities
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].vx[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].vy[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].vz[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    
    // Particle masses
    for (int i_patch = 0 ; i_patch < n_patches ; i_patch++) {
          binary_file.write((char *) &patches[i_patch].mass[0], sizeof(double)*patches[i_patch].getParticleNumber());
    }
    
    binary_file.close();
    
}
