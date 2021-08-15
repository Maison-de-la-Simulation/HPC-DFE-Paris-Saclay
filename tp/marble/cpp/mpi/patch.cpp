#include "patch.h"

// Constructor for Patch
Patch::Patch() {
    
    // Prepare the buffers for exchange in each direction
    exchange.resize(27);
    
};


Patch::Patch( int number )
{
    radius = 0.02;
    x.resize(number,0);
    y.resize(number,0);
    z.resize(number,0);
    vx.resize(number,0);
    vy.resize(number,0);
    vz.resize(number,0);
    mass.resize(number,0);
    
    // Prepare the buffers for exchange in each direction
    exchange.resize(27);
    
};

// Destructor
Patch::~Patch() {
    
};

// This function initializes the patch topology :
// - number of patches in each direction
// - id and coordinates of all patches
void Patch::initTopology(struct Parameters params) {
    
    // Patch index
    this->id = params.rank;
    this->id_x = params.rank_coordinates[2];
    this->id_y = params.rank_coordinates[1];
    this->id_z = params.rank_coordinates[0];
    
    // Local boundaries computation
    this->patch_x_length = (params.xmax - params.xmin) / params.n_patches_x;
    this->patch_y_length = (params.ymax - params.ymin) / params.n_patches_y;
    this->patch_z_length = (params.zmax - params.zmin) / params.n_patches_z;
    
    if (params.n_patches != params.number_of_ranks) {
        std::cerr << " CONFIGURATION ERROR: the number of ranks must equal the number of ranks." << std::endl;
        exit(0);
    }
    
    if ((patch_x_length <= 2*radius || patch_y_length <= 2*radius || patch_z_length <= 2*radius)) {
        std::cerr << " CONFIGURATION ERROR: a patch must be larger than a particle diameter." << std::endl;
        exit(0);
    }
    
    xmin = id_x * patch_x_length;
    xmax = (id_x+1) * patch_x_length;
    ymin = id_y * patch_y_length;
    ymax = (id_y+1) * patch_y_length;
    zmin = id_z * patch_z_length;
    zmax = (id_z+1) * patch_z_length;
    
    // std::cerr << "Patch id: " << id << " (" << id_x << ", " << id_y << ", " << id_z << ")"
    //           << " neighbors: ";
    // Computation of the neighbor patch index
    neighbor_indexes.resize(27);
    
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
                
                int k = ix+ 1 + (iy+1) * 3 + (iz+1) * 9;
                
                neighbor_indexes[k] = getNeighborIndex(params, ix, iy, iz);
                // if (neighbor_indexes[k] >= 0) {
                //     std::cerr << neighbor_indexes[k] << " (" << ix << " " << iy << " " << iz << ")" << ", ";
                // }
            }
        }
    }
    
    // Flag to know if the patch is at the boundary
    this->at_mx_boundary = (id_x == 0);
    this->at_my_boundary = (id_y == 0);
    this->at_mz_boundary = (id_z == 0);
    this->at_px_boundary = (id_x == params.n_patches_x-1);
    this->at_py_boundary = (id_y == params.n_patches_y-1);
    this->at_pz_boundary = (id_z == params.n_patches_z-1);
    
};

// Initialize the particles following a random pattern
void Patch::initParticles(struct Parameters params) {
    
    int local_number = (int) std::round(params.number / params.n_patches);
    
    radius = params.radius;
    x.resize(local_number,0);
    y.resize(local_number,0);
    z.resize(local_number,0);
    vx.resize(local_number,0);
    vy.resize(local_number,0);
    vz.resize(local_number,0);
    mass.resize(local_number,0);
    
    // Management of the offset for the particle positions
    double offset_mx = 0;
    double offset_my = 0;
    double offset_mz = 0;
    double offset_px = 0;
    double offset_py = 0;
    double offset_pz = 0;
    
    if (at_mx_boundary) offset_mx = radius;
    if (at_my_boundary) offset_my = radius;
    if (at_mz_boundary) offset_mz = radius;
    if (at_px_boundary) offset_px = radius;
    if (at_py_boundary) offset_py = radius;
    if (at_pz_boundary) offset_pz = radius;
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(params.rank); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1);
    
    if (params.overlap > 0) {
        // Position initialization while checking that 2 particles do not overlap
        for (int ip = 0 ; ip < local_number ; ip++) {
        
            x[ip] = dis(gen) * ( (xmax - offset_px) - (xmin + offset_mx) ) + (xmin + offset_mx);
            y[ip] = dis(gen) * ( (ymax - offset_py) - (ymin + offset_my) ) + (ymin + offset_my);
            z[ip] = dis(gen) * ( (zmax - offset_pz) - (zmin + offset_mz) ) + (zmin + offset_mz);
        }
    } else {
        // Position initialization while checking that 2 particles do not overlap
        for (int ip = 0 ; ip < local_number ; ip++) {
            
            bool position_validated = false;
            double distance;
            int ip2;
            
            // While the position is not validated, we regenerate the coordinates
            while(!position_validated) {
                
                x[ip] = dis(gen) * ( (xmax - offset_px) - (xmin + offset_mx) ) + (xmin + offset_mx);
                y[ip] = dis(gen) * ( (ymax - offset_py) - (ymin + offset_my) ) + (ymin + offset_my);
                z[ip] = dis(gen) * ( (zmax - offset_pz) - (zmin + offset_mz) ) + (zmin + offset_mz);
                
                position_validated = true;
                
                ip2 = 0;
                
                // Here we check that ip and ip2 are not in contact
                // If it is the case then the position is validated for ip
                while (position_validated && ip2 < ip) {
                    
                    distance = std::pow(x[ip] - x[ip2],2) + std::pow(y[ip] - y[ip2],2) + std::pow(z[ip] - z[ip2],2);
                    
                    if (distance < 4*radius*radius) {
                        position_validated = false;
                    }
                    
                    ip2++;
                    
                }
            }
        }
    }
    
    // Mass and velocity initilization
    for (int ip = 0 ; ip < local_number ; ip++) {
    
        mass[ip] = dis(gen) * (params.mass_max - params.mass_min) + params.mass_min;
    
        double v     = dis(gen) * (params.vmax - params.vmin) + params.vmin;
        
        vx[ip] =  dis(gen)-0.5;
        vy[ip] =  dis(gen)-0.5;
        vz[ip] =  dis(gen)-0.5;
        
        v = v/std::sqrt(vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip]);
        
        vx[ip] *= v;
        vy[ip] *= v;
        vz[ip] *= v;
    }
    
};

// Initilization for tests
void Patch::initParticlesTest(struct Parameters params) {
    radius = params.radius;
    x.resize(2,0);
    y.resize(2,0);
    z.resize(2,0);
    vx.resize(2,0);
    vy.resize(2,0);
    vz.resize(2,0);
    mass.resize(2,0);
    
    x[0] = 0.1;
    y[0] = 0.505;
    z[0] = 0.5;
    x[1] = 0.9;
    y[1] = 0.5;
    z[1] = 0.5;
    vx[0] = 0.07;
    vy[0] = 0;
    vz[0] = 0;
    vx[1] = -0.02;
    vy[1] = 0;
    vz[1] = 0;
    mass[0] = 0.1;
    mass[1] = 0.2;
}

// Push the particles using the velocity during the given time step
// This function solves the equations of movements
void Patch::push(struct Parameters params) {
    
    double kx, ky, kz;
    double inverse_mass;
    double inverse_norm_v;
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
    
        inverse_mass = 1/mass[ip];
        
        inverse_norm_v = 1/std::sqrt(vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip]);
        
        kx = vx[ip] * inverse_norm_v;
        ky = vy[ip] * inverse_norm_v;
        kz = vz[ip] * inverse_norm_v;
    
        // Gravity and friction
    
        vx[ip] += (params.gravity_x - params.air_damping * kx * inverse_mass) * params.step;
        vy[ip] += (params.gravity_y - params.air_damping * ky * inverse_mass) * params.step;
        vz[ip] += (params.gravity_z - params.air_damping * kz * inverse_mass) * params.step;
    
        // Position update
    
        x[ip] += vx[ip] * params.step;
        y[ip] += vy[ip] * params.step;
        z[ip] += vz[ip] * params.step;
        
    }
    
}

void Patch::walls(struct Parameters params, Walls walls) {
    
    for (int iw = 0 ; iw < walls.size() ; iw++) {
        
        struct Wall wall = *walls(iw);
        
        
        for (int ip = 0 ; ip < x.size() ; ip++) {
        
            double d = distance(ip,wall) - radius;
            
            // if (id == 18 && ip == 29 ) {
            //     std::cout << " i: " << ip
            //               << " x: " << x[ip]
            //               << " y: " << y[ip]
            //               << " z: " << z[ip]
            //               << " vx: " << vx[ip]
            //               << " distance: " << d
            //                << std::endl;
            // }
            
            // Check if the particle has collided with one of the wall
            if ( d < 0 ) {
                
                double dot_product = (vx[ip]*wall.normal[0] + vy[ip]*wall.normal[1] + vz[ip]*wall.normal[2]);
                
                // On vérifie que la particule rentre bien dans le mur
                if (dot_product < 0) {
                
                    // Update of the first part of the position
                    double time_to_collision = std::abs(d) / std::abs(dot_product);
                    
                    // std::cout
                    //           << " time to collision: " << time_to_collision
                    //           << " dot_product: " << dot_product
                    //            << std::endl;
                    
                    x[ip] -= time_to_collision * vx[ip] ;
                    y[ip] -= time_to_collision * vy[ip] ;
                    z[ip] -= time_to_collision * vz[ip] ;
                    
                    // update of the velocities (reflection)
                    double coef = std::sqrt(1 - wall.damping);
                    vx[ip] = coef * ( vx[ip] - 2 * dot_product * wall.normal[0] );
                    vy[ip] = coef * ( vy[ip] - 2 * dot_product * wall.normal[1] );
                    vz[ip] = coef * ( vz[ip] - 2 * dot_product * wall.normal[2] );
                    
                    // Update of the second part of the position
                    x[ip] += time_to_collision * vx[ip] ;
                    y[ip] += time_to_collision * vy[ip] ;
                    z[ip] += time_to_collision * vz[ip] ;
                }
            }
            
        }
    }
}

// This function manages the collisions between particles
int Patch::collisions(struct Parameters params) {
    
    int i1;
    int i2;
    int collision_counter = 0;
    
    // Ensure that a particle is involved in 1 collision only
    bool not_collided[x.size()];
    for (i1 = 0 ; i1 < x.size() ; i1++) {
        not_collided[i1] = true;
    }
    
    for (i1 = 0 ; i1 < x.size() ; i1++) {
        
        if (not_collided[i1]) {
        
            for (i2 = i1+1 ; i2 < x.size() ; i2++) {
                
                if (not_collided[i2]) {
                
                    // Relative position
                    double xr = x[i1] - x[i2];
                    double yr = y[i1] - y[i2];
                    double zr = z[i1] - z[i2];
                    
                    // Relative velocity
                    double vxr = vx[i1] - vx[i2];
                    double vyr = vy[i1] - vy[i2];
                    double vzr = vz[i1] - vz[i2];
                    
                    // Relative distance between particles
                    double distance         = xr*xr +yr*yr +zr*zr;
                    double inverse_distance = 1/std::sqrt(distance);
                    
                    // Norm of the relative velocity
                    double v                = vxr*vxr +vyr*vyr +vzr*vzr;
                    
                    // Relative velocity projected on the vector x2 - x1
                    // If positive, the particles are getting closer
                    double projected_v      = (xr*vxr + yr*vyr + zr*vzr) * inverse_distance;
                    
                    // We consider that the particles have collided only if the distance
                    // is below their radius and they are getting closer
                    if (distance < 4*radius*radius && projected_v > 0) {
                        
                        not_collided[i1] = false;
                        not_collided[i2] = false;
                        collision_counter ++;
                        
                        // Computation of the exact collision time
                        double b            = -2*(vxr*xr + vyr*yr + vzr*zr);
                        double c            = distance - 4*radius*radius;
                        double delta        = b*b - 4*v*c;
                        double dt_collision = (-b + std::sqrt(delta)) / (2*v);
                        
                        double kx = xr * inverse_distance;
                        double ky = yr * inverse_distance;
                        double kz = zr * inverse_distance;
                        
                        double inverse_mass = 1/ ( mass[i1] + mass[i2]);
                        
                        double a1 = 2 * (kx * vxr + ky * vyr + kz * vzr) * mass[i2] * inverse_mass;
                        double a2 = 2 * (kx * vxr + ky * vyr + kz * vzr) * mass[i1] * inverse_mass;
                        
                        // Position at exact collision time
                        x[i1] -= vx[i1] * dt_collision;
                        y[i1] -= vy[i1] * dt_collision;
                        z[i1] -= vz[i1] * dt_collision;
                        
                        x[i2] -= vx[i2] * dt_collision;
                        y[i2] -= vy[i2] * dt_collision;
                        z[i2] -= vz[i2] * dt_collision;
                        
                        double inelastic_coef = std::sqrt(1 - params.damping);
                        
                        // Velocity upadte after colliion
                        vx[i1] = inelastic_coef*(vx[i1] - a1 * kx);
                        vy[i1] = inelastic_coef*(vy[i1] - a1 * ky);
                        vz[i1] = inelastic_coef*(vz[i1] - a1 * kz);
                        
                        vx[i2] = inelastic_coef*(vx[i2] + a2 * kx);
                        vy[i2] = inelastic_coef*(vy[i2] + a2 * ky);
                        vz[i2] = inelastic_coef*(vz[i2] + a2 * kz);
                        
                        // Position after collision
                        x[i1] += vx[i1] * dt_collision;
                        y[i1] += vy[i1] * dt_collision;
                        z[i1] += vz[i1] * dt_collision;
                        
                        x[i2] += vx[i2] * dt_collision;
                        y[i2] += vy[i2] * dt_collision;
                        z[i2] += vz[i2] * dt_collision;
                        
                    } // end if
                }
            } // end for(i2 = i1+1 ; i2 < x.size() ; i2++)...
        } // end if
    }
    
    return collision_counter;
    
}

// Multiple collison iterations
void Patch::multipleCollisions(struct Parameters params) {
    
    collision_counter = 0;
    int subcollision_counter = 0;
    int collision_iteration = 0;
    
    do {
        subcollision_counter = collisions(params);
        collision_counter += subcollision_counter;
        collision_iteration++;
    } while (subcollision_counter > 0 && collision_iteration < params.collision);
    
}

// Determine particles to exchange
void Patch::computeExchangeBuffers(struct Parameters params) {
    
    exchange_counter = 0;
    
    int x_shift = 0;
    int y_shift = 0;
    int z_shift = 0;
    int ibuffer;
    int neighbor_index ;
    
    // We reitnitialize the buffers
    for (ibuffer = 0 ; ibuffer < 27 ; ibuffer++) {
        exchange[ibuffer].x.resize(0);
        exchange[ibuffer].y.resize(0);
        exchange[ibuffer].z.resize(0);
        exchange[ibuffer].vx.resize(0);
        exchange[ibuffer].vy.resize(0);
        exchange[ibuffer].vz.resize(0);
        exchange[ibuffer].mass.resize(0);
    }
    
    // We resize the mask to mark the particles that go away from this patch
    mask.resize(x.size());
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
    
        mask[ip] = true;
                    
        // We get the patch where the particle is located
        getParticlePatchShift(ip, x_shift, y_shift, z_shift);
    
        // Get the neighbor to be sure the particle is in the domain
        neighbor_index = getNeighborIndex(params, x_shift, y_shift, z_shift);
    
        // If the particle ip leaves the patch...
        if (!(x_shift == 0 && y_shift == 0 && z_shift == 0) && neighbor_index != MPI_PROC_NULL) {
    
            // First we determine the buffer where to store this particle
            ibuffer = (z_shift+1) * 9 + (y_shift+1)*3 + (x_shift+1);
    
            // We put the leaving particle in the correct buffer
            exchange[ibuffer].x.push_back(x[ip]);
            exchange[ibuffer].y.push_back(y[ip]);
            exchange[ibuffer].z.push_back(z[ip]);
            exchange[ibuffer].vx.push_back(vx[ip]);
            exchange[ibuffer].vy.push_back(vy[ip]);
            exchange[ibuffer].vz.push_back(vz[ip]);
            exchange[ibuffer].mass.push_back(mass[ip]);
            
            mask[ip] = false;
            
            exchange_counter += 1;
    
        }
    }
}

void Patch::computeExchangeBuffersPerDirection(struct Parameters params, int direction) {
    
    int shift = 0;
    int ibuffer;
    int neighbor_index ;
    
    // We reitnitialize the buffers
    for (ibuffer = 0 ; ibuffer < 2 ; ibuffer++) {
        exchange[ibuffer].x.resize(0);
        exchange[ibuffer].y.resize(0);
        exchange[ibuffer].z.resize(0);
        exchange[ibuffer].vx.resize(0);
        exchange[ibuffer].vy.resize(0);
        exchange[ibuffer].vz.resize(0);
        exchange[ibuffer].mass.resize(0);
    }
    
    // We resize the mask to mark the particles that go away from this patch
    mask.resize(x.size());
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
    
        mask[ip] = true;
                    
        // We get the patch where the particle is located
        if (direction == 0) {
            if (x[ip] < xmin) {
                shift = -1;
            } else if (x[ip] >= xmax) {
                shift = 1;
            } else {
                shift = 0;
            }
            
            // Get the neighbor to be sure the particle is in the domain
            neighbor_index = getNeighborIndex(params, shift, 0, 0);
                
        } else if (direction == 1) {
            if (y[ip] < ymin) {
                shift = -1;
            } else if (y[ip] >= ymax) {
                shift = 1;
            } else {
                shift = 0;
            }
            
            // Get the neighbor to be sure the particle is in the domain
            neighbor_index = getNeighborIndex(params, 0, shift, 0);
            
        } else if (direction == 2) {
            if (z[ip] <zmin) {
                shift = -1;
            } else if (z[ip] >= zmax) {
                shift = 1;
            } else {
                shift = 0;
            }
            
            // Get the neighbor to be sure the particle is in the domain
            neighbor_index = getNeighborIndex(params, 0, 0, shift);
            
        }

    
        // If the particle ip leaves the patch...
        if (!(shift == 0) && neighbor_index != MPI_PROC_NULL) {
    
            // First we determine the buffer where to store this particle
            if (shift < 0) ibuffer = 0;
            if (shift > 0) ibuffer = 1;
    
            // We put the leaving particle in the correct buffer
            exchange[ibuffer].x.push_back(x[ip]);
            exchange[ibuffer].y.push_back(y[ip]);
            exchange[ibuffer].z.push_back(z[ip]);
            exchange[ibuffer].vx.push_back(vx[ip]);
            exchange[ibuffer].vy.push_back(vy[ip]);
            exchange[ibuffer].vz.push_back(vz[ip]);
            exchange[ibuffer].mass.push_back(mass[ip]);
            
            mask[ip] = false;
            exchange_counter += 1;
    
        }
    }
}

// Delete the particles leaving particles marked by the mask vector
void Patch::deleteLeavingParticles() {
    int ip = 0;
    int last_ip = x.size()-1;
    while (ip <= last_ip) {
        
        if (mask[ip] == false) {
            x[ip] = x[last_ip];
            y[ip] = y[last_ip];
            z[ip] = z[last_ip];
            vx[ip] = vx[last_ip];
            vy[ip] = vy[last_ip];
            vz[ip] = vz[last_ip];
            mass[ip] = mass[last_ip];
            mask[ip] = mask[last_ip];
            last_ip--;
            
            // std::cerr << " id: " << id
            //           << " ip: " << ip
            //           << std::endl;
            
        } else {
            ip++;
        }
    }
    
    x.resize(last_ip+1);
    y.resize(last_ip+1);
    z.resize(last_ip+1);
    vx.resize(last_ip+1);
    vy.resize(last_ip+1);
    vz.resize(last_ip+1);
    mass.resize(last_ip+1);
}

// MPI communication of particles using MPI_Isend and MPI_Irecv
void Patch::receivedParticlesFromNeighborsIsend(std::vector<Patch> & patches, struct Parameters params) {
    
    int send_index;
    int recv_index;
    
    int send_buf_index;
    int recv_buf_index;
    
    int send_buf_size[26];
    int recv_buf_size[26];
    
    int tag = 0;
    MPI_Request request0[2*26];
    MPI_Status status[2];
    
    for (int i = 0 ; i < 26 ; i ++) {
        recv_buf_size[i] = 0;
    }
    
    // for (int i = 0 ; i < 14 ; i ++) {
    //     request[i] = MPI_REQUEST_NULL;
    // }
    
    // Loops over the directions
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
                
                // Neighbor index for sending the data
                
                send_index = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                // The case send_index means iz=ix=iy= 0
                if (send_index != 13) {
                
                    // Neighbor index for receiving the data
                    
                    recv_index = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    
                    send_buf_index = send_index;
                    recv_buf_index = recv_index;
                    if (send_index > 13) send_buf_index -= 1;
                    if (recv_index > 13) recv_buf_index -= 1;
                    
                    // Communication of the number of particles to exchange
                    
                    send_buf_size[send_buf_index] = patches[0].exchange[send_index].x.size(); // Contains the number of particles to send
                    recv_buf_size[recv_buf_index] = 0;                                              // contains the number of particles to receive
                    
                    MPI_Isend(&send_buf_size[send_buf_index],1,MPI_INT,neighbor_indexes[send_index],tag,params.cartesian_communicator,&request0[0 + 2*send_buf_index]);

                    MPI_Irecv(&recv_buf_size[recv_buf_index],1,MPI_INT,neighbor_indexes[recv_index],tag,params.cartesian_communicator,&request0[1 + 2*send_buf_index]);

                    
                }
            }
        }
    }
    
    MPI_Waitall(2*26,request0,MPI_STATUSES_IGNORE);
    
    int total_recv = 0;
    for (int i = 0 ; i < 26 ; i++) {
        total_recv += recv_buf_size[i];
    }
    
    int displacement[26];
    displacement[0] = 0;
    for (int i = 1 ; i < 26 ; i++) {
        displacement[i] = displacement[i-1] + recv_buf_size[i-1];
    }
    
    
    // std::cerr << params.rank << " " << total_recv << std::endl;
    
    int current_size = x.size();
                    
                    // if (send_buf_size > 0 || recv_buf_size > 0) {
                    //     //if (params.rank==2) {
                    //         std::cerr << " " << params.iteration
                    //     << " rank: " << params.rank << " send size: " << send_buf_size << " - recv size: " << recv_buf_size
                    //                                 << " - ix: " <<  ix << " " << iy << " " << iz
                    //                                 << " - neighbor index for send: " << neighbor_indexes[send_index]
                    //                                 << " - neighbor index for recv: " << neighbor_indexes[recv_index]
                    //                                 << " - number of particles: " << current_size
                    //                                   << std::endl;
                    //     //}
                    // }
                    
                    
    if (total_recv > 0) {
        
        x.resize(current_size + total_recv);
        y.resize(current_size + total_recv);
        z.resize(current_size + total_recv);
        vx.resize(current_size + total_recv);
        vy.resize(current_size + total_recv);
        vz.resize(current_size + total_recv);
        mass.resize(current_size + total_recv);
        
    }
    
    MPI_Request request[14*26];
    
    // Loops over the directions
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
    
                // Neighbor index for sending the data
                
                send_index = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                // The case send_index means iz=ix=iy= 0
                if (send_index != 13) {
                
                    // Neighbor index for receiving the data
                    
                    recv_index = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    
                    send_buf_index = send_index;
                    recv_buf_index = recv_index;
                    if (send_index > 13) send_buf_index -= 1;
                    if (recv_index > 13) recv_buf_index -= 1;
    
                    // if (send_buf_size[send_buf_index]) {
                        MPI_Isend(&patches[0].exchange[send_index].x[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[0 + 14*recv_buf_index]);
                        MPI_Isend(&patches[0].exchange[send_index].y[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[1+ 14*recv_buf_index]);
                        MPI_Isend(&patches[0].exchange[send_index].z[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[2+ 14*recv_buf_index]);
                        
                        MPI_Isend(&patches[0].exchange[send_index].vx[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[3+ 14*recv_buf_index]);
                        MPI_Isend(&patches[0].exchange[send_index].vy[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[4+ 14*recv_buf_index]);
                        MPI_Isend(&patches[0].exchange[send_index].vz[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[5+ 14*recv_buf_index]);
                        
                        MPI_Isend(&patches[0].exchange[send_index].mass[0],send_buf_size[send_buf_index],MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[6+ 14*recv_buf_index]);
                    //}
        
                    //if (recv_buf_size[recv_buf_index]) {
                        
                        int position = current_size + displacement[recv_buf_index];
                        
                        MPI_Irecv(&patches[0].x[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[7+ 14*recv_buf_index]);
                        MPI_Irecv(&patches[0].y[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[8+ 14*recv_buf_index]);
                        MPI_Irecv(&patches[0].z[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[9+ 14*recv_buf_index]);
                        
                        MPI_Irecv(&patches[0].vx[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[10+ 14*recv_buf_index]);
                        MPI_Irecv(&patches[0].vy[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[11+ 14*recv_buf_index]);
                        MPI_Irecv(&patches[0].vz[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[12+ 14*recv_buf_index]);
                        
                        MPI_Irecv(&patches[0].mass[position],recv_buf_size[recv_buf_index],MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[13+ 14*recv_buf_index]);
                    //}

                }
            }
        }
    }
    MPI_Waitall(14*26,request,MPI_STATUSES_IGNORE);
}

// MPI communication of particles using MPI_Isend and MPI_Irecv with packing of the particle's properties
void Patch::receivedParticlesFromNeighborsIsendPack(std::vector<Patch> & patches, struct Parameters params) {
    
    int send_index;
    int recv_index;
    
    int send_buf_index;
    int recv_buf_index;
    
    int send_buf_size[26];
    int recv_buf_size[26];
    
    int tag = 0;
    MPI_Request request0[2*26];
    MPI_Status status[2];
    
    for (int i = 0 ; i < 26 ; i ++) {
        recv_buf_size[i] = 0;
    }
    
    // for (int i = 0 ; i < 14 ; i ++) {
    //     request[i] = MPI_REQUEST_NULL;
    // }
    
    // Loops over the directions
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
                
                // Neighbor index for sending the data
                
                send_index = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                // The case send_index means iz=ix=iy= 0
                if (send_index != 13) {
                
                    // Neighbor index for receiving the data
                    
                    recv_index = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    
                    send_buf_index = send_index;
                    recv_buf_index = recv_index;
                    if (send_index > 13) send_buf_index -= 1;
                    if (recv_index > 13) recv_buf_index -= 1;
                    
                    // Communication of the number of particles to exchange
                    
                    send_buf_size[send_buf_index] = patches[0].exchange[send_index].x.size(); // Contains the number of particles to send
                    recv_buf_size[recv_buf_index] = 0;                                              // contains the number of particles to receive
                    
                    MPI_Isend(&send_buf_size[send_buf_index],1,MPI_INT,neighbor_indexes[send_index],tag,params.cartesian_communicator,&request0[0 + 2*send_buf_index]);

                    MPI_Irecv(&recv_buf_size[recv_buf_index],1,MPI_INT,neighbor_indexes[recv_index],tag,params.cartesian_communicator,&request0[1 + 2*send_buf_index]);

                    
                }
            }
        }
    }
    
    MPI_Waitall(2*26,request0,MPI_STATUSES_IGNORE);
    
    int total_recv = 0;
    for (int i = 0 ; i < 26 ; i++) {
        total_recv += recv_buf_size[i];
    }
    
    int displacement[26];
    displacement[0] = 0;
    for (int i = 1 ; i < 26 ; i++) {
        displacement[i] = displacement[i-1] + recv_buf_size[i-1];
    }
    
    
    // std::cerr << params.rank << " " << total_recv << std::endl;
    
    int current_size = x.size();
                    
                    // if (send_buf_size > 0 || recv_buf_size > 0) {
                    //     //if (params.rank==2) {
                    //         std::cerr << " " << params.iteration
                    //     << " rank: " << params.rank << " send size: " << send_buf_size << " - recv size: " << recv_buf_size
                    //                                 << " - ix: " <<  ix << " " << iy << " " << iz
                    //                                 << " - neighbor index for send: " << neighbor_indexes[send_index]
                    //                                 << " - neighbor index for recv: " << neighbor_indexes[recv_index]
                    //                                 << " - number of particles: " << current_size
                    //                                   << std::endl;
                    //     //}
                    // }
                    
                    
    if (total_recv > 0) {
        
        x.resize(current_size + total_recv);
        y.resize(current_size + total_recv);
        z.resize(current_size + total_recv);
        vx.resize(current_size + total_recv);
        vy.resize(current_size + total_recv);
        vz.resize(current_size + total_recv);
        mass.resize(current_size + total_recv);
        
    }
    
    MPI_Request request[2*26];
    
    for (int i = 0 ; i < 2*26 ; i ++) {
        request[i] = MPI_REQUEST_NULL;
    }
    
    double * recv_pack = new double[total_recv*7];
    
    double ** send_pack = new double *[26];
    for (send_buf_index = 0 ; send_buf_index < 26 ; send_buf_index++) {
        send_pack[send_buf_index] = new double[send_buf_size[send_buf_index]*7];
    }
    
    // Loops over the directions
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
    
                // Neighbor index for sending the data
                
                send_index = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                // The case send_index means iz=ix=iy= 0
                if (send_index != 13) {
                
                    // Neighbor index for receiving the data
                    
                    recv_index = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    
                    send_buf_index = send_index;
                    recv_buf_index = recv_index;
                    if (send_index > 13) send_buf_index -= 1;
                    if (recv_index > 13) recv_buf_index -= 1;
    
    
                    for (int i = 0 ; i < send_buf_size[send_buf_index]; i++) {
                        send_pack[send_buf_index][i] = patches[0].exchange[send_index].x[i];
                        send_pack[send_buf_index][i+send_buf_size[send_buf_index]] = patches[0].exchange[send_index].y[i];
                        send_pack[send_buf_index][i+2*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].z[i];
                        send_pack[send_buf_index][i+3*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vx[i];
                        send_pack[send_buf_index][i+4*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vy[i];
                        send_pack[send_buf_index][i+5*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vz[i];
                        send_pack[send_buf_index][i+6*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].mass[i];
                    }
                    
                    // if (params.rank == 1) {
                    //     for (int i = 0 ; i < send_buf_size[send_buf_index]; i++) {
                    //         std::cerr << " " << send_pack[i]
                    //                   << " " << send_pack[i+send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+2*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+3*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+4*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+5*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+6*send_buf_size[send_buf_index]]
                    //                   << std::endl;
                    //     }
                    // }
    
                    if (send_buf_size[send_buf_index] > 0) {
                        MPI_Isend(&send_pack[send_buf_index][0],send_buf_size[send_buf_index]*7,MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[2*recv_buf_index]);
                    }
        
                    if (recv_buf_size[recv_buf_index] > 0) {
                        MPI_Irecv(&recv_pack[displacement[recv_buf_index]*7],recv_buf_size[recv_buf_index]*7,MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[1+2*recv_buf_index]);
                    }
                    

                }
            }
        }
    }
    
    MPI_Waitall(2*26,request,MPI_STATUSES_IGNORE);
    
    for (recv_buf_index = 0 ; recv_buf_index < 26; recv_buf_index++) {
        int position = current_size + displacement[recv_buf_index];
        for (int i = 0 ; i < recv_buf_size[recv_buf_index]; i++) {
            
            patches[0].x[position + i] =  recv_pack[i];
            patches[0].y[position + i] = recv_pack[i+recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].z[position + i] = recv_pack[i+2*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vx[position + i] = recv_pack[i+3*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vy[position + i] = recv_pack[i+4*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vz[position + i] = recv_pack[i+5*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].mass[position + i] = recv_pack[i+6*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];

        }
    }
    
    delete [] recv_pack;
    
    for (send_buf_index = 0 ; send_buf_index < 26 ; send_buf_index++) {
        delete [] send_pack[send_buf_index];
    }
    delete [] send_pack;
    
}

// MPI communication of particles using MPI_Sendrecv
void Patch::receivedParticlesFromNeighborsSendrecv(std::vector<Patch> & patches, struct Parameters params) {
    
    // Loops over the directions
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
                
                // Neighbor index for sending the data
                
                int send_index = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                // The case send_index means iz=ix=iy= 0
                if (send_index != 13) {
                
                    // Neighbor index for receiving the data
                    
                    int recv_index = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    
                    int send_buf_index = send_index;
                    if (send_index >= 13) send_buf_index -= 1;
                    
                    // Communication of the number of particles to exchange
                    
                    int send_buf_size = patches[0].exchange[send_index].x.size(); // Contains the number of particles to send
                    int recv_buf_size = 0;                                              // contains the number of particles to receive
                    int tag = 0;
                    
                    MPI_Sendrecv(&send_buf_size,1,MPI_INT, neighbor_indexes[send_index], tag, &recv_buf_size, 1, MPI_INT, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    
                    int current_size = x.size();
                    
                    // if (send_buf_size > 0 || recv_buf_size > 0) {
                    //     //if (params.rank==2) {
                    //         std::cerr << " " << params.iteration
                    //     << " rank: " << params.rank << " send size: " << send_buf_size << " - recv size: " << recv_buf_size
                    //                                 << " - ix: " <<  ix << " " << iy << " " << iz
                    //                                 << " - neighbor index for send: " << neighbor_indexes[send_index]
                    //                                 << " - neighbor index for recv: " << neighbor_indexes[recv_index]
                    //                                 << " - number of particles: " << current_size
                    //                                   << std::endl;
                    //     //}
                    // }
                    
                    
                    if (recv_buf_size > 0) {
                        
                        x.resize(current_size + recv_buf_size);
                        y.resize(current_size + recv_buf_size);
                        z.resize(current_size + recv_buf_size);
                        vx.resize(current_size + recv_buf_size);
                        vy.resize(current_size + recv_buf_size);
                        vz.resize(current_size + recv_buf_size);
                        mass.resize(current_size + recv_buf_size);
                        
                    }
                    
                    MPI_Sendrecv(&patches[0].exchange[send_index].x[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].x[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    MPI_Sendrecv(&patches[0].exchange[send_index].y[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].y[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    MPI_Sendrecv(&patches[0].exchange[send_index].z[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].z[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                                 
                    MPI_Sendrecv(&patches[0].exchange[send_index].vx[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].vx[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    MPI_Sendrecv(&patches[0].exchange[send_index].vy[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].vy[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    MPI_Sendrecv(&patches[0].exchange[send_index].vz[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].vz[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                    
                    MPI_Sendrecv(&patches[0].exchange[send_index].mass[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                                 &patches[0].mass[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                }
            }
        }
    }
}

void Patch::receivedParticlesPerDirectionSendrecv(std::vector<Patch> & patches, struct Parameters params, int direction) {
    
    // Loops over the directions
    for (int i = -1 ; i <= 1 ; i++) {
        
        if (i != 0) {
        
            // Neighbor index for sending the data
            int send_index;
            int recv_index ;
            int send_buf_index;
            
            // ix+1 + (iy+1) * 3 + (iz+1) * 9;
            // (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
            
            if (direction == 0) {
                send_index = i+1 + 12;
                recv_index = (-1*i) + 13;
            } else if (direction == 1) {
                send_index = 1 + (i+1)*3 + 9;
                recv_index = 1 + (-1*i+1) * 3 + 9;
            } else if (direction == 2) {
                send_index = 4 + 9 * (i+1);
                recv_index = 4 + 9 * (-1*i+1);
            }
            
            if (i < 0) send_buf_index = 0;
            if (i > 0) send_buf_index = 1;
                    
            int send_buf_size = patches[0].exchange[send_index].x.size(); // Contains the number of particles to send
            int recv_buf_size = 0;                                            // contains the number of particles to receive
            int tag = 0;
                        
            MPI_Sendrecv(&send_buf_size,1,MPI_INT, neighbor_indexes[send_index], tag, &recv_buf_size, 1, MPI_INT, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                        
            int current_size = x.size();

            if (recv_buf_size > 0) {
                
                x.resize(current_size + recv_buf_size);
                y.resize(current_size + recv_buf_size);
                z.resize(current_size + recv_buf_size);
                vx.resize(current_size + recv_buf_size);
                vy.resize(current_size + recv_buf_size);
                vz.resize(current_size + recv_buf_size);
                mass.resize(current_size + recv_buf_size);
                
            }
                    
            MPI_Sendrecv(&patches[0].exchange[send_index].x[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].x[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            MPI_Sendrecv(&patches[0].exchange[send_index].y[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].y[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            MPI_Sendrecv(&patches[0].exchange[send_index].z[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].z[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
                         
            MPI_Sendrecv(&patches[0].exchange[send_index].vx[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].vx[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            MPI_Sendrecv(&patches[0].exchange[send_index].vy[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].vy[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            MPI_Sendrecv(&patches[0].exchange[send_index].vz[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].vz[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            
            MPI_Sendrecv(&patches[0].exchange[send_index].mass[0],send_buf_size,MPI_DOUBLE, neighbor_indexes[send_index], tag,
                         &patches[0].mass[current_size], recv_buf_size, MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, MPI_STATUS_IGNORE);
            }
    }
}



void Patch::receivedParticlesPerDirectionIsendPack(std::vector<Patch> & patches, struct Parameters params, int direction) {
    
    int send_index;
    int recv_index;
    
    int send_buf_index;
    int recv_buf_index;
    
    int send_buf_size[2];
    int recv_buf_size[2];
    
    int tag = 0;
    MPI_Request request0[4];
    
    for (int i = 0 ; i < 2 ; i ++) {
        recv_buf_size[i] = 0;
    }
    
    // for (int i = 0 ; i < 14 ; i ++) {
    //     request[i] = MPI_REQUEST_NULL;
    // }
    
    // Loops over the directions
    for (int i = -1 ; i <= 1 ; i++) {
                
        if (i != 0) {
            
            // ix+1 + (iy+1) * 3 + (iz+1) * 9;
            // (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
            
            if (direction == 0) {
                send_index = i+1 + 12;
                recv_index = (-1*i) + 13;
            } else if (direction == 1) {
                send_index = 1 + (i+1)*3 + 9;
                recv_index = 1 + (-1*i+1) * 3 + 9;
            } else if (direction == 2) {
                send_index = 4 + 9 * (i+1);
                recv_index = 4 + 9 * (-1*i+1);
            }
            
            if (i < 0) {
                send_buf_index = 0;
                recv_buf_index = 0;
            }
            if (i > 0) {
                send_buf_index = 1;
                recv_buf_index = 1;
            }
                    
            // Communication of the number of particles to exchange
            
            send_buf_size[send_buf_index] = patches[0].exchange[send_index].x.size(); // Contains the number of particles to send
            recv_buf_size[recv_buf_index] = 0;                                              // contains the number of particles to receive
                    
            MPI_Isend(&send_buf_size[send_buf_index],1,MPI_INT,neighbor_indexes[send_index],tag,params.cartesian_communicator,&request0[0 + 2*send_buf_index]);

            MPI_Irecv(&recv_buf_size[recv_buf_index],1,MPI_INT,neighbor_indexes[recv_index],tag,params.cartesian_communicator,&request0[1 + 2*send_buf_index]);

        }
    }
    
    MPI_Waitall(4,request0,MPI_STATUSES_IGNORE);
    
    int total_recv = 0;
    for (int i = 0 ; i < 2 ; i++) {
        total_recv += recv_buf_size[i];
    }
    
    int displacement[2];
    displacement[0] = 0;
    for (int i = 1 ; i < 2 ; i++) {
        displacement[i] = displacement[i-1] + recv_buf_size[i-1];
    }
    
    
    // std::cerr << params.rank << " " << total_recv << std::endl;
    
    int current_size = x.size();
                    
                    // if (send_buf_size > 0 || recv_buf_size > 0) {
                    //     //if (params.rank==2) {
                    //         std::cerr << " " << params.iteration
                    //     << " rank: " << params.rank << " send size: " << send_buf_size << " - recv size: " << recv_buf_size
                    //                                 << " - ix: " <<  ix << " " << iy << " " << iz
                    //                                 << " - neighbor index for send: " << neighbor_indexes[send_index]
                    //                                 << " - neighbor index for recv: " << neighbor_indexes[recv_index]
                    //                                 << " - number of particles: " << current_size
                    //                                   << std::endl;
                    //     //}
                    // }
                    
                    
    if (total_recv > 0) {
        
        x.resize(current_size + total_recv);
        y.resize(current_size + total_recv);
        z.resize(current_size + total_recv);
        vx.resize(current_size + total_recv);
        vy.resize(current_size + total_recv);
        vz.resize(current_size + total_recv);
        mass.resize(current_size + total_recv);
        
    }
    
    MPI_Request request[4];
    
    for (int i = 0 ; i < 4 ; i ++) {
        request[i] = MPI_REQUEST_NULL;
    }
    
    double * recv_pack = new double[total_recv*7];
    
    double ** send_pack = new double *[2];
    for (send_buf_index = 0 ; send_buf_index < 2 ; send_buf_index++) {
        send_pack[send_buf_index] = new double[send_buf_size[send_buf_index]*7];
    }
    
    // Loops over the directions
    for (int i = -1 ; i <= 1 ; i++) {
    
        if (i != 0) {
            
            // ix+1 + (iy+1) * 3 + (iz+1) * 9;
            // (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
            
            if (direction == 0) {
                send_index = i+1 + 12;
                recv_index = (-1*i) + 13;
            } else if (direction == 1) {
                send_index = 1 + (i+1)*3 + 9;
                recv_index = 1 + (-1*i+1) * 3 + 9;
            } else if (direction == 2) {
                send_index = 4 + 9 * (i+1);
                recv_index = 4 + 9 * (-1*i+1);
            }
            
            if (i < 0) {
                send_buf_index = 0;
                recv_buf_index = 0;
            }
            if (i > 0) {
                send_buf_index = 1;
                recv_buf_index = 1;
            }
    
            for (int k = 0 ; k < send_buf_size[send_buf_index]; k++) {
                send_pack[send_buf_index][k] = patches[0].exchange[send_index].x[k];
                send_pack[send_buf_index][k+send_buf_size[send_buf_index]] = patches[0].exchange[send_index].y[k];
                send_pack[send_buf_index][k+2*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].z[k];
                send_pack[send_buf_index][k+3*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vx[k];
                send_pack[send_buf_index][k+4*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vy[k];
                send_pack[send_buf_index][k+5*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].vz[k];
                send_pack[send_buf_index][k+6*send_buf_size[send_buf_index]] = patches[0].exchange[send_index].mass[k];
            }
                    
                    // if (params.rank == 1) {
                    //     for (int i = 0 ; i < send_buf_size[send_buf_index]; i++) {
                    //         std::cerr << " " << send_pack[i]
                    //                   << " " << send_pack[i+send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+2*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+3*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+4*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+5*send_buf_size[send_buf_index]]
                    //                   << " " << send_pack[i+6*send_buf_size[send_buf_index]]
                    //                   << std::endl;
                    //     }
                    // }
    
            if (send_buf_size[send_buf_index] > 0) {
                MPI_Isend(&send_pack[send_buf_index][0],send_buf_size[send_buf_index]*7,MPI_DOUBLE,neighbor_indexes[send_index], tag,params.cartesian_communicator, &request[2*recv_buf_index]);
            }

            if (recv_buf_size[recv_buf_index] > 0) {
                MPI_Irecv(&recv_pack[displacement[recv_buf_index]*7],recv_buf_size[recv_buf_index]*7,MPI_DOUBLE, neighbor_indexes[recv_index], tag, params.cartesian_communicator, &request[1+2*recv_buf_index]);
            }
        }
    }
    
    MPI_Waitall(4,request,MPI_STATUSES_IGNORE);
    
    for (recv_buf_index = 0 ; recv_buf_index < 2; recv_buf_index++) {
        int position = current_size + displacement[recv_buf_index];
        for (int i = 0 ; i < recv_buf_size[recv_buf_index]; i++) {
            
            patches[0].x[position + i] =  recv_pack[i];
            patches[0].y[position + i] = recv_pack[i+recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].z[position + i] = recv_pack[i+2*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vx[position + i] = recv_pack[i+3*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vy[position + i] = recv_pack[i+4*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].vz[position + i] = recv_pack[i+5*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];
            patches[0].mass[position + i] = recv_pack[i+6*recv_buf_size[recv_buf_index] + displacement[recv_buf_index]*7];

        }
    }
    
    delete [] recv_pack;
    
    for (send_buf_index = 0 ; send_buf_index < 2 ; send_buf_index++) {
        delete [] send_pack[send_buf_index];
    }
    delete [] send_pack;
    
    
    
}

void Patch::exchangeParticlesDiagonalTrick(std::vector<Patch> & patches, struct Parameters params) {

    exchange_counter = 0;

    computeExchangeBuffersPerDirection(params, 0);
    deleteLeavingParticles();
    receivedParticlesPerDirectionSendrecv(patches, params,0);
    
    computeExchangeBuffersPerDirection(params, 1);
    deleteLeavingParticles();
    receivedParticlesPerDirectionSendrecv(patches, params,1);
    
    computeExchangeBuffersPerDirection(params, 2);
    deleteLeavingParticles();
    receivedParticlesPerDirectionSendrecv(patches, params,2);
    
}

void Patch::exchangeParticlesDiagonalTrickIsendPack(std::vector<Patch> & patches, struct Parameters params) {

    exchange_counter = 0;

    computeExchangeBuffersPerDirection(params, 0);
    deleteLeavingParticles();
    receivedParticlesPerDirectionIsendPack(patches, params,0);
    
    computeExchangeBuffersPerDirection(params, 1);
    deleteLeavingParticles();
    receivedParticlesPerDirectionIsendPack(patches, params,1);
    
    computeExchangeBuffersPerDirection(params, 2);
    deleteLeavingParticles();
    receivedParticlesPerDirectionIsendPack(patches, params,2);
    
}

// Compute the distance between the specified particle and the given wall
double Patch::distance(int ip, struct Wall wall) {
    return wall.normal[0] * x[ip] + wall.normal[1] * y[ip] + wall.normal[2] * z[ip] + wall.d ;
}

// Return the total energy
double Patch::getTotalEnergy() {
    
    double total_energy = 0;
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
        total_energy += 0.5 * mass[ip] * squareVelocity(ip);
    }
    
    return total_energy;
    
}

// Return the maximal particle velocity
double Patch::getMaxVelocity() {
    
    double max_velocity = 0;
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
        max_velocity = std::max(max_velocity, squareVelocity(ip));
    }
    
    return std::sqrt(max_velocity);
}

// Return the number of particles
int Patch::getParticleNumber() {
    return x.size();
}

// Return the number of collisions
int Patch::getCollisionNumber() {
    return collision_counter;
}

// Return the number of collisions
int Patch::getExchangeNumber() {
    return exchange_counter;
}

// Return the index of the requested neighbor
// If the neighbor does not exist the function returns -1
// x_shift : relative displacement regarding the current patch coordinates in the x direction
// y_shift : relative displacement regarding the current patch coordinates in the y direction
// z_shift : relative displacement regarding the current patch coordinates in the z direction
int Patch::getNeighborIndex(struct Parameters params, int x_shift, int y_shift, int z_shift) {
    
    int index = MPI_PROC_NULL;
    
    if ((id_x + x_shift >= 0) && (id_x + x_shift < params.n_patches_x ) &&
        (id_y + y_shift >= 0) && (id_y + y_shift < params.n_patches_y ) &&
        (id_z + z_shift >= 0) && (id_z + z_shift < params.n_patches_z )) {
            
        int index_2;
        
        patchCoordinatesToIndex(params, index_2, id_x + x_shift, id_y + y_shift, id_z + z_shift);
        
        index = index_2;
    }
    
    return index;
    
}

void Patch::patchCoordinatesToIndex(struct Parameters params, int & id, int id_x, int id_y, int id_z) {
    id = params.topology_map[id_z *(params.n_patches_y*params.n_patches_x) + id_y * params.n_patches_x + id_x];
}

// Check that all particles are in the domain
void Patch::checkParticlesInDomain(struct Parameters params) {
    
    for (int ip = 0 ; ip < x.size() ; ip++) {
        if (x[ip] < params.xmin || x[ip] > params.xmax ||
            y[ip] < params.ymin || y[ip] > params.ymax ||
            z[ip] < params.zmin || z[ip] > params.zmax) {
            std::cerr << " In patch: "<< id << ", problem with particle: " << ip << std::endl;
        }
    }
}
