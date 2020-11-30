#include "particles.h"

// Constructor for Particles
Particles::Particles() {};


Particles::Particles( unsigned int number )
{
    radius = 0.02;
    x.resize(number,0);
    y.resize(number,0);
    z.resize(number,0);
    vx.resize(number,0);
    vy.resize(number,0);
    vz.resize(number,0);
    mass.resize(number,0);
};

// Destructor
Particles::~Particles() {
    
};

// Initialize the particles following a random pattern
void Particles::initialization(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties) {
    
    radius = particle_properties.radius;
    x.resize(particle_properties.number,0);
    y.resize(particle_properties.number,0);
    z.resize(particle_properties.number,0);
    vx.resize(particle_properties.number,0);
    vy.resize(particle_properties.number,0);
    vz.resize(particle_properties.number,0);
    mass.resize(particle_properties.number,0);
    
    if (2*particle_properties.vmax * time.step > radius) {
        std::cerr << " WARNING: a particle cannot cross more than a radius during a time step." << std::endl;
        std::cerr << " Maximal distanced: " << 2*particle_properties.vmax * time.step << " / radius: " << radius << std::endl;
        exit(0);
    }
    
    // Position initialization while checking that 2 particles do not overlap
    for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
        
        bool position_validated = false;
        double distance;
        unsigned int ip2;
        
        // While the position is not validated, we regenerate the coordinates
        while(!position_validated) {
            
            x[ip] = drand48() * ( (domain_properties.xmax - radius) - (domain_properties.xmin + radius) ) + (domain_properties.xmin + radius);
            y[ip] = drand48() * ( (domain_properties.ymax - radius) - (domain_properties.ymin + radius) ) + (domain_properties.ymin + radius);
            z[ip] = drand48() * ( (domain_properties.zmax - radius) - (domain_properties.zmin + radius) ) + (domain_properties.zmin + radius);
            
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
    for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
    
        mass[ip] = drand48() * (particle_properties.mass_max - particle_properties.mass_min) + particle_properties.mass_min;
    
        double v     = drand48() * (particle_properties.vmax - particle_properties.vmin) + particle_properties.vmin;
        double theta = drand48() * 2 * M_PI;
        double phi   = drand48() * M_PI;
    
        vx[ip] = v * std::cos(theta)*std::cos(phi);
        vy[ip] = v * std::sin(theta)*std::cos(phi);
        vz[ip] = v * std::sin(phi);
    
    }
};

// Initilization for tests
void Particles::initialization_test(struct DomainProperties domain_properties, struct TimeProperties time, struct ParticleProperties particle_properties) {
    radius = particle_properties.radius;
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
void Particles::push(double time_step, struct DomainProperties domain_properties) {
    
    double kx, ky, kz;
    double inverse_mass;
    double inverse_norm_v;
    
    for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
    
        inverse_mass = 1/mass[ip];
        
        inverse_norm_v = 1/std::sqrt(vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip]);
        
        kx = vx[ip] * inverse_norm_v;
        ky = vy[ip] * inverse_norm_v;
        kz = vz[ip] * inverse_norm_v;
    
        // Gravity and friction
    
        vx[ip] += (domain_properties.gravity_x - domain_properties.air_damping * kx * inverse_mass) * time_step;
        vy[ip] += (domain_properties.gravity_y - domain_properties.air_damping * ky * inverse_mass) * time_step;
        vz[ip] += (domain_properties.gravity_z - domain_properties.air_damping * kz * inverse_mass) * time_step;
    
        // Position update
    
        x[ip] += vx[ip] * time_step;
        y[ip] += vy[ip] * time_step;
        z[ip] += vz[ip] * time_step;
        
        // std::cout << " i: " << ip
        //           << " x: " << x[ip]
        //           << " y: " << y[ip]
        //           << " z: " << z[ip]
        //           << " vx: " << vx[ip]
        //            << std::endl;
        
    }
    
    // std::cout << std::endl;
}

void Particles::walls(double time_step, Walls walls) {
    
    for (unsigned int iw = 0 ; iw < walls.size() ; iw++) {
        
        struct Wall wall = *walls(iw);
        
        for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
        
            double d = distance(ip,wall) - radius;
        
            // std::cout << " i: " << ip
            //           << " x: " << x[ip]
            //           << " y: " << y[ip]
            //           << " z: " << z[ip]
            //           << " vx: " << vx[ip]
            //           << " distance: " << d
            //            << std::endl;
        
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
unsigned int Particles::collisions(struct TimeProperties time, struct ParticleProperties particle_properties) {
    
    unsigned int i1;
    unsigned int i2;
    unsigned int collision_counter = 0;
    
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
                        
                        double inelastic_coef = std::sqrt(1 - particle_properties.damping);
                        
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
unsigned int Particles::multipleCollisions(struct TimeProperties time_properties, struct ParticleProperties particle_properties) {
    unsigned int collision_counter       = 0;
    if (particle_properties.collision) {
        unsigned int subcollision_counter    = 0;
        unsigned int collision_iteration     = 0;
        unsigned int max_collision_iteration = 10;
        
        do {
            subcollision_counter = collisions(time_properties,particle_properties);
            collision_counter += subcollision_counter;
            collision_iteration++;
        } while (subcollision_counter > 0 && collision_iteration < max_collision_iteration);
        
    }
    return collision_counter;
}

// Compute the distance between the specified particle and the given wall
double Particles::distance(unsigned int ip, struct Wall wall) {
    return wall.normal[0] * x[ip] + wall.normal[1] * y[ip] + wall.normal[2] * z[ip] + wall.d ;
}

// Return the total energy
double Particles::totalEnergy() {
    
    double total_energy = 0;
    
    for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
        total_energy += 0.5 * mass[ip] * squareVelocity(ip);
    }
    
    return total_energy;
    
}

// Return the maximal particle velocity
double Particles::maxVelocity() {
    
    double max_velocity = 0;
    
    for (unsigned int ip = 0 ; ip < x.size() ; ip++) {
        max_velocity = std::max(max_velocity, squareVelocity(ip));
    }
    
    return std::sqrt(max_velocity);
}

// Write the particle diagnostics
void Particles::writeDiags(struct TimeProperties time_properties, struct DiagProperties diag_properties) {
    if (time_properties.iteration%diag_properties.output_period == 0) {
        if (diag_properties.hdf5) {
            writeHDF5(time_properties.iteration);
        }
        if (diag_properties.vtk) {
            writeVTK(time_properties.iteration);
        }
    }
}

// Write the particle properties in a hdf5 file
void Particles::writeHDF5(unsigned int iteration) {
    
    hid_t       file, space, dset;
    herr_t      status;
    
    std::string data_set;
    
    std::string file_name = "diags/particles_" + std::to_string(iteration) + ".h5";
    
    // Creation of the file
    file = H5Fcreate (file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    // Create dataspace.  Setting maximum size to NULL sets the maximum
    // size to be the current size.
    hsize_t     dims[1] = {x.size()};
    space = H5Screate_simple (1, dims, NULL);
    
    // Creation of the datasets
    data_set = "mass";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &mass[0]);
    
    data_set = "x";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &x[0]);
    
    data_set = "y";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &y[0]);
    
    data_set = "z";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &z[0]);
    
    data_set = "vx";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vx[0]);
    
    data_set = "vy";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vy[0]);
    
    data_set = "vz";
    dset = H5Dcreate (file, data_set.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vz[0]);
    
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Fclose (file);
        
}

// Write the particle properties in a vitk file
void Particles::writeVTK(unsigned int iteration) {
    
    std::string file_name = "diags/particles_" + std::to_string(iteration) + ".vtk";
    
    std::ofstream vtk_file(file_name.c_str(), std::ios::out | std::ios::trunc);
    
    if (vtk_file)
    {
        vtk_file << "# vtk DataFile Version 3.0" << std::endl;
        vtk_file << "vtk output" << std::endl;
        vtk_file << "ASCII" << std::endl;
        vtk_file << "DATASET POLYDATA" << std::endl;
        
        unsigned int number = x.size();
        
        // Particle positions
        vtk_file << std::endl;
        vtk_file << "POINTS "<< number << " float" << std::endl;
        for(unsigned int ip = 0 ; ip < number ; ip++) {
          vtk_file << x[ip] << " ";
          vtk_file << y[ip] << " " ;
          vtk_file << z[ip] << std::endl ;
        }
        
        // Construction of the mass
        vtk_file << std::endl;
        vtk_file << "POINT_DATA " << number  << std::endl;
        vtk_file << "SCALARS mass float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(unsigned int ip = 0 ; ip < number ; ip++) {
          vtk_file << mass[ip] << " " ;
        }
        vtk_file << std::endl;
        
        // Construction of the energy
        vtk_file << std::endl;
        vtk_file << "SCALARS energy float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(unsigned int ip = 0 ; ip < number ; ip++) {
          vtk_file << 0.5*mass[ip]*(vx[ip]*vx[ip] + vy[ip]*vy[ip] + vz[ip]*vz[ip]) << " " ;
        }
        vtk_file << std::endl;
        
        // Construction of the radius
        vtk_file << std::endl;
        vtk_file << "SCALARS radius float" << std::endl;
        vtk_file << "LOOKUP_TABLE default" << std::endl;
        for(unsigned int ip = 0 ; ip < number ; ip++) {
          vtk_file << radius << " " ;
        }
        vtk_file << std::endl;
        
        vtk_file << std::endl;
        vtk_file << "VECTORS v float" << std::endl;
        for(unsigned int ip = 0 ; ip < number ; ip++) {
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
