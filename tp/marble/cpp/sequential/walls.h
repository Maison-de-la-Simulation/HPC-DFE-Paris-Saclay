#ifndef WALLS_H
#define WALLS_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parameters.h"
//#include "hdf5.h"

struct Wall {
    double normal[3];     // Normal vector
    double d;             // last coefficient for plan equation
    double normal_length; // normal length
    double damping;       // damping coefficient (0-1)
};

//! class ParticleInjector
class Walls
{
    public:
    
    //Particles constructor
    Walls();

    // Particles destructor
    ~Walls();
    
    // Addition of a new wall defined by a point and a normal
    void add( double point[3], double normal[3]);
    void add( double point[3], double normal[3], double damping);
    
    // Return the wall at index i
    inline struct Wall * operator() (unsigned int i) {
        return &list[i];
    };
    
    // Return the number of walls
    inline unsigned int size() {
        return list.size();
    }
    
    private:
        
    std::vector<struct Wall> list;

};

#endif
