#
# Makefile
#
#
#
# ______________________________________________________________________________

# Fortran compiler (MPI wrapper)
FC = gfortran

# Fortran compiler flags
FFLAGS= -fopenmp
# Libraries
LDFLAGS = -fopenmp
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

# Type of compiler
compiler = $(shell mpif90 -show | cut -f1 -d-)
ifneq (,$(findstring ifort,$(compiler)))
#    FFLAGS += -qopenmp
else ifneq (,$(findstring gfortran,$(compiler)))
#    FFLAGS += -fopenmp
endif

FFLAGS += -O3

# HDF5
# INCFLAGS += -I$(HDF5_DIR)/include/
# LDFLAGS += -L$(HDF5_DIR)/lib/ -lhdf5_fortran -lhdf5

# ______________________________________________________________________________
# Sources

#SRCS := $(shell find ./* -name \*.F90)
#OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.F90=.o))

SRCS := parameters.F90 physics.F90 diagnostics.F90 main.F90
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
	$(FC) $(OBJS) -o "$(APPNAME)$(APPNAME_EXTENSION)" $(LDFLAGS)

clean:
	@echo "Cleaning"
	rm -rf build
	rm -rf *.mod
