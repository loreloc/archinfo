
# minimum cmake version
cmake_minimum_required(VERSION 3.0.2)

# project name
project(archinfo)

# set C compiler flags
set(CMAKE_C_FLAGS "-Wall -O2")

# set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# archinfo executable file
add_executable(archinfo archinfo.c)

# link pthread library
if(UNIX)
	target_link_libraries(archinfo -lpthread)
endif()

# installation
install(TARGETS archinfo RUNTIME DESTINATION bin)

