#include "parameters.h"

// Function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct TimeProperties & time_properties,
                                                   struct ParticleProperties & particle_properties,
                                                   struct DomainProperties & domain_properties,
                                                   struct DiagProperties & diag_properties) {
    if (argc > 1) {
        unsigned int iarg = 1;
        std::string key;
        while(iarg < argc) {
            key = argv[iarg];
            if (key == "-t") {
                time_properties.final = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-it") {
                time_properties.final_iteration = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-np") {
                particle_properties.number = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-r") {
                particle_properties.radius = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-collision_damping") {
                particle_properties.damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-wall_damping") {
                domain_properties.wall_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-air_damping") {
                domain_properties.air_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-ve") {
                particle_properties.vmin = atof(argv[iarg+1]);
                particle_properties.vmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-mass") {
                particle_properties.mass_min = atof(argv[iarg+1]);
                particle_properties.mass_max = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-x") {
                domain_properties.xmin = atof(argv[iarg+1]);
                domain_properties.xmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-y") {
                domain_properties.ymin = atof(argv[iarg+1]);
                domain_properties.ymax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-z") {
                domain_properties.zmin = atof(argv[iarg+1]);
                domain_properties.zmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-gravity") {
                domain_properties.gravity_x = atof(argv[iarg+1]);
                domain_properties.gravity_y = atof(argv[iarg+2]);
                domain_properties.gravity_z = atof(argv[iarg+3]);
                iarg+=3;
            } else if (key == "-print") {
                diag_properties.print_period = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-diags") {
                diag_properties.output_period = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-co") {
                int arg = atoi(argv[iarg+1]);
                particle_properties.collision = (arg > 0);
                iarg++;
            } else {
                iarg++;
            }
        }
    }
}
