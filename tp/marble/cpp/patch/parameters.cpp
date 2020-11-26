#include "parameters.h"

// Function to read the external arguments
void commandLineArguments(int argc, char * argv[], struct Parameters & params) {
    if (argc > 1) {
        int iarg = 1;
        std::string key;
        while(iarg < argc) {
            key = argv[iarg];
            if (key == "-t") {
                params.final = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-it") {
                params.final_iteration = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-np") {
                params.number = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-r") {
                params.radius = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-collision_damping") {
                params.damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-wall_damping") {
                params.wall_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-air_damping") {
                params.air_damping = atof(argv[iarg+1]);
                iarg++;
            } else if (key == "-velocity") {
                params.vmin = atof(argv[iarg+1]);
                params.vmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-mass") {
                params.mass_min = atof(argv[iarg+1]);
                params.mass_max = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-x") {
                params.xmin = atof(argv[iarg+1]);
                params.xmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-y") {
                params.ymin = atof(argv[iarg+1]);
                params.ymax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-z") {
                params.zmin = atof(argv[iarg+1]);
                params.zmax = atof(argv[iarg+2]);
                iarg+=2;
            } else if (key == "-gravity") {
                params.gravity_x = atof(argv[iarg+1]);
                params.gravity_y = atof(argv[iarg+2]);
                params.gravity_z = atof(argv[iarg+3]);
                iarg+=3;
            } else if (key == "-patch") {
                params.n_patches_x = atoi(argv[iarg+1]);
                params.n_patches_y = atoi(argv[iarg+2]);
                params.n_patches_z = atoi(argv[iarg+3]);
                iarg+=3;
            } else if (key == "-print") {
                params.print_period = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-diags") {
                params.output_period = atoi(argv[iarg+1]);
                iarg++;
            } else if (key == "-collision") {
                params.collision = atoi(argv[iarg+1]);
                if (params.collision < 0) params.collision = 0;
                iarg++;
            } else if (key == "-overlap") {
                params.overlap = atoi(argv[iarg+1]);
                if (params.overlap < 0) params.overlap = 0;
                iarg++;
            } else {
                iarg++;
            }
        }
    }
}
