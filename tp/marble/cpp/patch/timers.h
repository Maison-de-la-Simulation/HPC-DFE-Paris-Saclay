#ifndef TIMERS_H
#define TIMERS_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <sys/time.h>
#include "parameters.h"

//! class Timers
class Timers
{
    public:

    // Constructor
    Timers( );

    // Destructor
    ~Timers();
    
    // Add a timer
    void add(std::string name);
    
    // Start a timer
    void start(std::string name);
    
    // Stop a timer
    void stop(std::string name);
    
    // Print all timers
    void print();
    
    // Return the timer index
    int index(std::string name);
    
    private:
        
    // Array to store the timers
    std::vector <double> accumulated_times;
    
    // Array to store temporary values
    std::vector <double> temporary_times;
    
    // Timer's names
    std::vector <std::string> names;
    
};

#endif
