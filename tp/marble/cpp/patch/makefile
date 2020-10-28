# ______________________________________________________________________________
#
# Makefile
# ______________________________________________________________________________

# C++ compiler or MPI wrapper
CPP = g++

# Include flags
INCFLAGS =
# Where the compiled files are located
BUILD_DIR = ./build
# Final application name
APPNAME = executable
APPNAME_EXTENSION =
# Architecture flags
ARCHI_FLAGS=
# HDF5 location
# HDF5_DIR=/home/tp-home012/mlobet/Public/hdf5-1.10.4/build

# ______________________________________________________________________________
# Manage options

# Optimization
CPPFLAGS += -O3

# HDF5
INCFLAGS += -I$(HDF5_HOME)/include/
LDFLAGS += -L$(HDF5_HOME)/lib/ -lhdf5

# ______________________________________________________________________________
# Sources

#SRCS := $(shell find ./* -name \*.F90)
#OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.F90=.o))

SRCS := parameters.cpp timers.cpp walls.cpp patch.cpp particles.cpp main.cpp
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

# Compilation
$(BUILD_DIR)/%.o:%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@ $(INCFLAGS)

all: mkdir build

mkdir:
	@mkdir -p build

# Link the main program
build: $(OBJS)
	@echo "Linking $@"
	$(CPP) $(OBJS) -o "$(APPNAME)$(APPNAME_EXTENSION)" $(LDFLAGS)

clean:
	@echo "Cleaning"
	rm -rf build
	rm -rf *.mod
