# ______________________________________________________________________________
#
# Makefile
# ______________________________________________________________________________

# C++ compiler or MPI wrapper
CPP = mpic++

# Include flags
INCFLAGS =
# Where the compiled files are located
BUILD_DIR = ./build
# Final application name
APPNAME = executable

# ______________________________________________________________________________
# Manage options

CPPLAGS += -O3 -std=c++11
LDFLAGS =
# ______________________________________________________________________________
# Sources

SRCS := main.cpp
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

# Compilation
$(BUILD_DIR)/%.o:%.cpp
	$(CPP) $(CPPLAGS) -c $< -o $@ $(INCFLAGS)

all: mkdir build

mkdir:
	@mkdir -p build

# Link the main program
build: $(OBJS)
	@echo "Linking $@"
	$(CPP) $(OBJS) -o "$(APPNAME)" $(LDFLAGS)

clean:
	@echo "Cleaning"
	rm -rf build
	rm -rf *.mod
