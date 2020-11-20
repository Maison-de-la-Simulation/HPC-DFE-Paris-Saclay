#include "patch.h"

// Constructor for Patch
Patch::Patch() {
    
    // Prepare the buffers for exchange in each direction
    exchange.resize(26);
    
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
    exchange.resize(26);
    
};

// Destructor
Patch::~Patch() {
    
};

// This function initializes the patch topology :
// - number of patches in each direction
// - id and coordinates of all patches
void Patch::initTopology(struct Parameters params,
                        int id) {
    
    // Patch index
    this->id = id;
    
    // Local indexes in the cartesian topology
    // x is the fastest direction
    patchIndexToCoordinates(params, id, this->id_x, this->id_y, this->id_z);
    
    // Local boundaries computation
    this->patch_x_length = (params.xmax - params.xmin) / params.n_patches_x;
    this->patch_y_length = (params.ymax - params.ymin) / params.n_patches_y;
    this->patch_z_length = (params.zmax - params.zmin) / params.n_patches_z;
    
    if (patch_x_length <= 2*radius || patch_y_length <= 2*radius || patch_z_length <= 2*radius) {
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
    
    // std::cerr << std::endl;
    
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
    
    int local_number = params.number / params.n_patches;
    
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
    
    // Position initialization while checking that 2 particles do not overlap
    for (int ip = 0 ; ip < local_number ; ip++) {
        
        bool position_validated = false;
        double distance;
        int ip2;
        
        // std::cerr << "ip: " << ip
        //           << " local_number: " << local_number
        //           << " radius: " << radius
        //           << " xmin: " << xmin << " " << xmax << " " << patch_x_length
        //           << std::endl;
        
        // While the position is not validated, we regenerate the coordinates
        while(!position_validated) {
            
            x[ip] = drand48() * ( (xmax - offset_px) - (xmin + offset_mx) ) + (xmin + offset_mx);
            y[ip] = drand48() * ( (ymax - offset_py) - (ymin + offset_my) ) + (ymin + offset_my);
            z[ip] = drand48() * ( (zmax - offset_pz) - (zmin + offset_mz) ) + (zmin + offset_mz);
            
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
    
    // Mass and velocity initilization
    for (int ip = 0 ; ip < local_number ; ip++) {
    
        mass[ip] = drand48() * (params.mass_max - params.mass_min) + params.mass_min;
    
        double v     = drand48() * (params.vmax - params.vmin) + params.vmin;
        double theta = drand48() * 2 * M_PI;
        double phi   = drand48() * M_PI;
    
        vx[ip] = v * std::cos(theta)*std::cos(phi);
        vy[ip] = v * std::sin(theta)*std::cos(phi);
        vz[ip] = v * std::sin(phi);
    
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
void Patch::push(struct Parameters params)  {
    
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
        
        // std::cout << " i: " << ip
        //           << " x: " << x[ip]
        //           << " y: " << y[ip]
        //           << " z: " << z[ip]
        //           << " vx: " << vx[ip]
        //            << std::endl;
        
    }
    
    // std::cout << std::endl;
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
    int max_collision_iteration = 10;
    
    do {
        subcollision_counter = collisions(params);
        collision_counter += subcollision_counter;
        collision_iteration++;
    } while (subcollision_counter > 0 && collision_iteration < max_collision_iteration);
    
}

// Determine particles to exchange
void Patch::computeExchangeBuffers(struct Parameters params) {
    
    int x_shift = 0;
    int y_shift = 0;
    int z_shift = 0;
    int ibuffer;
    int neighbor_index ;
    
    // We reitnitialize the buffers
    for (ibuffer = 0 ; ibuffer < 26 ; ibuffer++) {
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
    
        // If the particle ip leave the patch...
        if (!(x_shift == 0 && y_shift == 0 && z_shift == 0) && neighbor_index > 0) {
    
            // First we determine the buffer where to store this particle
            ibuffer = (z_shift+1) * 9 + (y_shift+1)*3 + (x_shift+1);
    
            // We remove the case zshift = yshift = xshift = 0
            if (ibuffer >= 13) ibuffer -= 1;
    
            // We put the leaving particle in the correct buffer
            exchange[ibuffer].x.push_back(x[ip]);
            exchange[ibuffer].y.push_back(y[ip]);
            exchange[ibuffer].z.push_back(z[ip]);
            exchange[ibuffer].vx.push_back(vx[ip]);
            exchange[ibuffer].vy.push_back(vy[ip]);
            exchange[ibuffer].vz.push_back(vz[ip]);
            exchange[ibuffer].mass.push_back(mass[ip]);
            
            // std::cerr << " id: " << id
            //           << " ip: " << ip
            //           << " ibuffer: " << ibuffer
            //           << " xshift: " << x_shift
            //           << " yshift: " << y_shift
            //           << " zshift: " << z_shift
            //           << std::endl;
            
            mask[ip] = false;
    
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

// Each patch copies the neighbor buffer in their inner particle list
void Patch::receivedParticlesFromNeighbors(std::vector<Patch> & patches) {
    
    // Loops over the neighbors
    for (int iz = -1 ; iz <= 1 ; iz++) {
        for (int iy = -1 ; iy <= 1 ; iy++) {
            for (int ix = -1 ; ix <= 1 ; ix++) {
                
                // Neighbor index
                int k = ix+1 + (iy+1) * 3 + (iz+1) * 9;
                
                if (k != 13 && neighbor_indexes[k] >= 0) {
                
                    // Exchange buffer index (we remove the case ix = iy = iz = 0)
                    int l = (-1*ix) + 1 + (-1*iy+1) * 3 + (-1*iz+1) * 9;
                    if (l >= 13) l-= 1;
                
                    
                    int exchange_size = patches[neighbor_indexes[k]].exchange[l].x.size();
                    
                    // if (id == 4) {
                    //     std::cerr << " ix: " << ix << " iy: " << iy << " iz: " << iz
                    //               << " neighbor_indexes[k]: " << neighbor_indexes[k]
                    //               << " exchange_size: " << exchange_size
                    //               << std::endl;
                    // }
                    
                    if (exchange_size > 0) {
                        
                        int current_size = x.size();
                        
                        // std::cerr << " k: " << k
                        //           << " id: " << id
                        //           << " l: " << l
                        //           << " ix: " << ix
                        //           << " iy: " << iy
                        //           << " iz: " << iz
                        //           << " neighbor_indexes[k]: " << neighbor_indexes[k]
                        //           << " exchange_size: " << exchange_size
                        //           << " current_size: " << current_size
                        //           << std::endl;
                        
                        x.resize(current_size + exchange_size);
                        y.resize(current_size + exchange_size);
                        z.resize(current_size + exchange_size);
                        vx.resize(current_size + exchange_size);
                        vy.resize(current_size + exchange_size);
                        vz.resize(current_size + exchange_size);
                        mass.resize(current_size + exchange_size);
                        
                        for (unsigned ip = 0 ; ip < exchange_size ; ip++) {
                            x[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].x[ip];
                            y[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].y[ip];
                            z[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].z[ip];
                            vx[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].vx[ip];
                            vy[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].vy[ip];
                            vz[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].vz[ip];
                            mass[current_size + ip] = patches[neighbor_indexes[k]].exchange[l].mass[ip];
                        }
                    }
                
                }
                
            }
        }
    }
    
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

// Return the index of the requested neighbor
// If the neighbor does not exist the function returns -1
// x_shift : relative displacement regarding the current patch coordinates in the x direction
// y_shift : relative displacement regarding the current patch coordinates in the y direction
// z_shift : relative displacement regarding the current patch coordinates in the z direction
int Patch::getNeighborIndex(struct Parameters params, int x_shift, int y_shift, int z_shift) {
    
    int index = -1;
    
    if ((id_x + x_shift >= 0) && (id_x + x_shift < params.n_patches_x ) &&
        (id_y + y_shift >= 0) && (id_y + y_shift < params.n_patches_y ) &&
        (id_z + z_shift >= 0) && (id_z + z_shift < params.n_patches_z )) {
        int index_2;
        
        patchCoordinatesToIndex(params, index_2, id_x + x_shift, id_y + y_shift, id_z + z_shift);
        
        index = index_2;
    }
    
    return index;
    
}

// Return in which patch the particle is located relatively to the current patch.
// xmin : shift = -1
// xmax : shift = 1
// ...
void Patch::getParticlePatchShift(int ip, int & x_shift, int & y_shift, int & z_shift) {
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
