#include "timers.h"

// Constructor for Particles
Timers::Timers() {
    
    // By default there are 2 timers :
    // - initilization
    // - main loop
    
    names.resize(2);
    temporary_times.resize(2);
    accumulated_times.resize(2);
    
    names[0] = "initialization";
    names[1] = "main loop";
    
    temporary_times[0] = 0;
    temporary_times[1] = 0;
    
    accumulated_times[0] = 0;
    accumulated_times[1] = 0;
};

// Destructor
Timers::~Timers() {
};

// Add a timer to the internal list
void Timers::add(std::string name) {
    names.push_back(name);
    
    temporary_times.push_back(0);
    accumulated_times.push_back(0);
}

// Start the specified timer
void Timers::start(std::string name) {
    
    #pragma omp master
    {
    
    int id = index(name);
    
    double time = omp_get_wtime();
    
    temporary_times[id] = time;
    
    }
    
}

// Stop the specified timer
void Timers::stop(std::string name) {
    
    #pragma omp master
    {
    
    int id = index(name);
    
    double time = omp_get_wtime();
    
    accumulated_times[id] += time - temporary_times[id];
    
    }
    
}

// Return the specicied timer index
int Timers::index(std::string name) {
    int index = 0;
    
    while(names[index] != name && index < names.size()) {
        index++;
    }
    
    if (index >= names.size()) {
        index = -1;
    }
    
    return index;
}

void Timers::print() {

    double percentage;
    
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << " TIMERS"<< std::endl;
    std::cout << " ------------------------------------ "<< std::endl;
    std::cout << "            code part |  time (s)  | percentage |"<< std::endl;
    std::cout << " ---------------------|------------|----------- |"<< std::endl;
    
    percentage = accumulated_times[0] / (accumulated_times[1] + accumulated_times[0]) * 100;
    std::cout << " " << std::setw(20) << names[0] ;
    std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << accumulated_times[0] ;
    std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
    std::cout << " | " ;
    std::cout << std::endl;
    
    percentage = accumulated_times[1] / (accumulated_times[1] + accumulated_times[0]) * 100;
    std::cout << " " << std::setw(20) << names[1] ;
    std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << accumulated_times[1] ;
    std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
    std::cout << " | " ;
    std::cout << std::endl;

    for (int i = 2 ; i < names.size() ; i++) {
        
        percentage = accumulated_times[i] / (accumulated_times[1] + accumulated_times[0]) * 100;
        
        std::cout << " " << std::setw(20) << names[i] ;
        std::cout << " | " << std::fixed << std::setprecision(6) << std::setw(10) << accumulated_times[i];
        std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(8) << percentage << " %";
        std::cout << " | " ;
        std::cout << std::endl;
    }
    
}
