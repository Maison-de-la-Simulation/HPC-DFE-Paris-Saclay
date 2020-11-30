#include "walls.h"

// Constructor for Particles
Walls::Walls( ) {
};

Walls::~Walls() {
};

void Walls::add(double point[3], double normal[3]) {
    add(point, normal, 0);
};

void Walls::add(double point[3], double normal[3], double damping) {
    
    // Temporary wall constructed from the input parameters
    struct Wall wall;
    wall.normal[0] = normal[0];
    wall.normal[1] = normal[1];
    wall.normal[2] = normal[2];
    wall.d = -(normal[0]*point[0] + normal[1]*point[1] + normal[2]*point[2]);
    
    // Normalization in case of
    double normal_length = std::sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    wall.normal[0] /= normal_length;
    wall.normal[1] /= normal_length;
    wall.normal[2] /= normal_length;
    
    // Absorption coefficient
    wall.damping = damping;
    
    // Addition of the wall to the list
    list.push_back(wall);
};
