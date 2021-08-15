#include "particles.h"

// Constructor for Particles
Particles::Particles(struct Parameters params)
{
    patches.resize(1);
    
};

// Destructor
Particles::~Particles() {
};

// Initialize the topology for each patch
void Particles::initTopology(struct Parameters & params) {
    
    if (params.number_of_ranks != params.n_patches_x * params.n_patches_y * params.n_patches_z) {
        std::cerr << " CONFIGURATION ERROR: the total number of patches must match the topology: " << params.number_of_ranks << " " << params.n_patches_x << " " << params.n_patches_y  << std::endl;
        exit(0);
    }
    
    params.ranks_per_direction[2] = params.n_patches_x;
    params.ranks_per_direction[1] = params.n_patches_y;
    params.ranks_per_direction[0] = params.n_patches_z;
    
    params.periodicity[0] = 0;
    params.periodicity[1] = 0;
    params.periodicity[2] = 0;
    params.reorganisation = 1;
    
    MPI_Cart_create(MPI_COMM_WORLD,
                         3,
                         params.ranks_per_direction,
                         params.periodicity,
                         params.reorganisation,
                         &params.cartesian_communicator);
                         
    MPI_Comm_rank( params.cartesian_communicator, &params.rank );
    
    MPI_Cart_coords( params.cartesian_communicator, params.rank, 3, params.rank_coordinates);
    
    params.topology_map = new int[params.number_of_ranks];
    
    int coordinates[3];
    for (int i = 0 ; i < params.number_of_ranks ; i++) {
        MPI_Cart_coords( params.cartesian_communicator, i, 3, coordinates);
        //if (params.rank == 0) std::cerr << "rank: " << i << " " << coordinates[0] << " " << coordinates[1] << " " << coordinates[2] << std::endl;
        params.topology_map[coordinates[0]*params.ranks_per_direction[1]*params.ranks_per_direction[2] + coordinates[1]*params.ranks_per_direction[2] + coordinates[2]] = i;
    }
    
    if (params.rank == 0) {

        std::cout << std::endl;
        std::cout <<  " Topology map: "<< std::endl;

        for(int iz = 0; iz < params.ranks_per_direction[0] ; iz++) {
            std::cout << "z = " << iz << std::endl;
            std::cout <<  " ---------------------------> x"<< std::endl;
            for(int iy = 0; iy < params.ranks_per_direction[1] ; iy++) {
                for(int ix = 0; ix < params.ranks_per_direction[2] ; ix++) {
                
                    std::cout << " | " << std::setw(3) << params.topology_map[iz*params.ranks_per_direction[1]*params.ranks_per_direction[2] + iy*params.ranks_per_direction[2] + ix] ;
                
                }
                std::cout << "" << std::endl;
            }
            std::cout << " v" << std::endl;
            std::cout << " y"  << std::endl;
        }
    }
    
    patches[0].initTopology(params);
    
};

// Initialize the particles for each patch
void Particles::initParticles(struct Parameters params) {

    if (2*params.vmax * params.step > params.radius && (params.collision > 0)) {
        std::cerr << " WARNING: a particle can cross more than a radius during a time step." << std::endl;
        std::cerr << " Maximal distanced: " << 2*params.vmax * params.step << " / radius: " << params.radius << std::endl;
        //exit(0);
    }

    patches[0].initParticles(params);
    
};

// Push the particles using the velocity during the given time step
// This function solves the equations of movements
void Particles::push(struct Parameters params) {
    
    patches[0].push(params);

}

// Applied the walls to the particles
void Particles::walls(struct Parameters params, Walls walls) {
    // We perform the walls several times in case of multiple rebound
    for (int iw = 0 ; iw < 3; iw++) {
            patches[0].walls(params, walls);
    }
}

// Multiple collison iterations
void Particles::multipleCollisions(struct Parameters params) {
    //
    // int * particle_per_rank = new int[params.number_of_ranks];
    //
    // int local = patches[0].getParticleNumber();
    //
    // MPI_Gather(&local,1,MPI_INT,particle_per_rank,1, MPI_INT,0,params.cartesian_communicator);
    //
    // if (params.rank==0) {
    //     for (int i = 0 ; i < params.number_of_ranks ; i++) {
    //         std::cout << " " << particle_per_rank[i];
    //     }
    //     std::cout << std::endl;
    // }
    //
    // delete [] particle_per_rank;
    
    if (params.collision) {
            patches[0].multipleCollisions(params);
    }
    
}

// Exchange particles between patches
void Particles::exchange(struct Parameters params) {
    
    if (params.exchange == 0) {
        patches[0].computeExchangeBuffers(params);
        patches[0].deleteLeavingParticles();
        patches[0].receivedParticlesFromNeighborsSendrecv(patches, params);
    } else if (params.exchange == 1) {
        patches[0].computeExchangeBuffers(params);
        patches[0].deleteLeavingParticles();
        patches[0].receivedParticlesFromNeighborsIsend(patches, params);
    } else if (params.exchange == 2) {
        patches[0].computeExchangeBuffers(params);
        patches[0].deleteLeavingParticles();
        patches[0].receivedParticlesFromNeighborsIsendPack(patches, params);
    }  else if (params.exchange == 3) {
        patches[0].exchangeParticlesDiagonalTrick(patches, params);
    } else if (params.exchange == 4) {
        patches[0].exchangeParticlesDiagonalTrickIsendPack(patches, params);
    }
    
}

// Return the total energy
void Particles::getTotalEnergy(struct Parameters params, double & total_energy) {
    
    double patch_energy = patches[0].getTotalEnergy();
    
    MPI_Reduce(&patch_energy, &total_energy, 1, MPI_DOUBLE, MPI_SUM, 0, params.cartesian_communicator);
    
}

// Return the maximal particle velocity
void Particles::getMaxVelocity(struct Parameters params, double & max_velocity) {
    
    double patch_max_velocity = patches[0].getMaxVelocity();
    max_velocity = 0;
    
    MPI_Reduce(&patch_max_velocity, &max_velocity, 1, MPI_DOUBLE, MPI_MAX, 0, params.cartesian_communicator);
}

// Return the total number of particles
void Particles::getTotalParticleNumber(struct Parameters params, int & total, int & imbalance) {
    
    int local = patches[0].getParticleNumber();
    int max;
    int min;
    
    MPI_Reduce(&local, &total, 1, MPI_INT, MPI_SUM, 0, params.cartesian_communicator);
    MPI_Reduce(&local, &max, 1, MPI_INT, MPI_MAX, 0, params.cartesian_communicator);
    MPI_Reduce(&local, &min, 1, MPI_INT, MPI_MIN, 0, params.cartesian_communicator);

    imbalance = max - min;

}

// Return the total number of collisions
void Particles::getTotalCollisionNumber(struct Parameters params, int & total) {
    
    int local = patches[0].getCollisionNumber();
    
    MPI_Reduce(&local, &total, 1, MPI_INT, MPI_SUM, 0, params.cartesian_communicator);

}

// Return the total number of exchanges
void Particles::getTotalExchangeNumber(struct Parameters params, int & total) {
    
    int local = patches[0].getExchangeNumber();
    
    MPI_Reduce(&local, &total, 1, MPI_INT, MPI_SUM, 0, params.cartesian_communicator);

}

// Write the particle diagnostics
// Ineficcient method: all particles are communicated to the rank 0 that writes all diags.
// However, very pedagogic to learn global communication
void Particles::writeDiags(struct Parameters params) {
    if (params.iteration%params.output_period == 0) {
        
        int total = 0;
        
        // For diagnostics
        
        int * local_particle_numbers = new int[params.n_patches];
        int * displacement = new int[params.n_patches];
        int local = patches[0].getParticleNumber();
        
        MPI_Gather(&local, 1, MPI_INT, &local_particle_numbers[0], 1, MPI_INT, 0, params.cartesian_communicator);
        
        if (params.rank==0) {
            for (int i = 0 ; i < params.n_patches ; i++) {
                total += local_particle_numbers[i];
            }
            displacement[0] = 0;
            for (int i = 1 ; i < params.n_patches ; i++) {
                displacement[i] = displacement[i-1] + local_particle_numbers[i-1];
            }
        }
        
        double * x = new double[total];
        double * y = new double[total];
        double * z = new double[total];
        
        double * vx = new double[total];
        double * vy = new double[total];
        double * vz = new double[total];
        
        double * mass = new double[total];
        
        MPI_Gatherv(&patches[0].x[0],local,MPI_DOUBLE, &x[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        MPI_Gatherv(&patches[0].y[0],local,MPI_DOUBLE, &y[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        MPI_Gatherv(&patches[0].z[0],local,MPI_DOUBLE, &z[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        
        MPI_Gatherv(&patches[0].vx[0],local,MPI_DOUBLE, &vx[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        MPI_Gatherv(&patches[0].vy[0],local,MPI_DOUBLE, &vy[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        MPI_Gatherv(&patches[0].vz[0],local,MPI_DOUBLE, &vz[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        
        MPI_Gatherv(&patches[0].mass[0],local,MPI_DOUBLE, &mass[0],local_particle_numbers, displacement, MPI_DOUBLE, 0, params.cartesian_communicator);
        
        if (params.vtk && params.rank==0) {
        
            writeVTK(params.iteration, total, x, y, z, vx, vy, vz, mass);
        }
        if (params.binary) {
            writeBinary(params.iteration, total, x, y, z, vx, vy, vz, mass);
        }
        // // if (diag_properties.hdf5) {
        // //     writeHDF5(params.iteration);
        // // }
        //
        delete [] x;
        delete [] y,z,vx,vy,vz,mass, local_particle_numbers, displacement;
        
    }
}


// Write the particle properties in a vitk file
void Particles::writeVTK(int iteration, int number_of_particles,
                        double * x, double * y, double * z,
                        double * vx, double * vy, double * vz, double * mass) {
    
    std::string file_name = "diags/particles_" + std::to_string(iteration) + ".vtk";
    
    std::ofstream vtk_file(file_name.c_str(), std::ios::out | std::ios::trunc);
    
    if (vtk_file)
    {
        vtk_file << "# vtk DataFile Version 3.0" << std::endl;
        vtk_file << "vtk output" << std::endl;
        vtk_file << "ASCII" << std::endl;
        vtk_file << "DATASET POLYDATA" << std::endl;
        
        // Particle positions
        vtk_file << std::endl;
        vtk_file << "POINTS "<< number_of_particles << " float" << std::endl;
        for(int ip = 0 ; ip < number_of_particles ; ip++) {
            vtk_file << x[ip] << " " << y[ip] << " " << z[ip] << std::endl ;
        }
        
        // Construction of the mass
        vtk_file << std::endl;
        vtk_file << "POINT_DATA " << number_of_particles  << std::endl;
        vtk_file << "SCALARS mass float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(int ip = 0 ; ip < number_of_particles ; ip++) {
            vtk_file << mass[ip] << " " ;
        }
        vtk_file << std::endl;
        
        // Construction of the energy
        vtk_file << std::endl;
        vtk_file << "SCALARS energy float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(int ip = 0 ; ip < number_of_particles ; ip++) {
            vtk_file << 0.5*mass[ip]*(vx[ip]*vx[ip] +
                                    vy[ip]*vy[ip] +
                                    vz[ip]*vz[ip]) << " " ;
        }
        vtk_file << std::endl;
        
        // Construction of the radius
        vtk_file << std::endl;
        vtk_file << "SCALARS radius float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(int ip = 0 ; ip < number_of_particles ; ip++) {
          vtk_file << patches[0].radius << " " ;
        }
        vtk_file << std::endl;
        
        // Construction of the speed vector
        vtk_file << std::endl;
        vtk_file << "VECTORS v float" << std::endl;
        for(int ip = 0 ; ip < number_of_particles ; ip++) {
            vtk_file << vx[ip] << " ";
            vtk_file << vy[ip] << " ";
            vtk_file << vz[ip] << " ";
        }
        
    }
    else
    {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }
    
}

// Write the particle properties in a binary file
void Particles::writeBinary(int iteration, int number_of_particles,
                        double * x, double * y, double * z,
                        double * vx, double * vy, double * vz, double * mass) {
    
    std::string file_name = "diags/particles_" + std::to_string(iteration) + ".bin";
    
    std::ofstream binary_file(file_name.c_str(), std::ios::out | std::ios::binary);
    
    if(!binary_file) {
        std::cerr << " Error while creating the file :" << file_name << std::endl;
    }
    
    binary_file.write((char *) &number_of_particles, sizeof(number_of_particles));
    binary_file.write((char *) &patches[0].radius, sizeof(double));
    
    // Particle positions
    binary_file.write((char *) &x[0], sizeof(double)*number_of_particles);
    binary_file.write((char *) &y[0], sizeof(double)*number_of_particles);
    binary_file.write((char *) &z[0], sizeof(double)*number_of_particles);
    
    // Particle velocities
    binary_file.write((char *) &vx[0], sizeof(double)*number_of_particles);
    binary_file.write((char *) &vy[0], sizeof(double)*number_of_particles);
    binary_file.write((char *) &vz[0], sizeof(double)*number_of_particles);
    
    binary_file.write((char *) &mass[0], sizeof(double)*number_of_particles);
    
    
    binary_file.close();
    
}
