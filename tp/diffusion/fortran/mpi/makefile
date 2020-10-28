#
# Makefile
#
#
#
# ______________________________________________________________________________

# Fortran compiler (MPI wrapper)
FC = mpif90

# Fortran compiler flags
FFLAGS=
# Libraries
LDFLAGS =
# Include flags
INCFLAGS =
# Where the compiled files are located
BUILD_DIR = ./build
# Final application name
APPNAME = executable
# Architecture flags
ARCHI_FLAGS=
# HDF5 location
# HDF5_DIR=/local/home/mlobet/Installs/hdf5-1.8.20-gcc7.1/build/

# ______________________________________________________________________________
# Manage options

# HDF5
INCFLAGS += -I$(HDF5_HOME)/include/
LDFLAGS += -L$(HDF5_HOME)/lib/ -lhdf5_fortran -lhdf5

# ______________________________________________________________________________
# Sources

#SRCS := $(shell find ./* -name \*.F90)
#OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.F90=.o))

SRCS := parameters.F90 tools.F90 timers.F90 parallelism.F90 physics.F90 diagnostics.F90 main.F90
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.F90=.o))

# Compilation
$(BUILD_DIR)/%.o $(BUILD_DIR)/%.mod:%.F90
	$(FC) $(FFLAGS) -c $< -o $@ $(INCFLAGS)

all: mkdir build

mkdir:
	@mkdir -p build

# Link the main program
build: $(OBJS)
	@echo "Linking $@"
	$(FC) $(OBJS) -o "$(APPNAME)" $(LDFLAGS)

clean:
	@echo "Cleaning"
	rm -rf build
	rm -rf *.mod
