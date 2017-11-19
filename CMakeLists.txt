
# minimum cmake version
cmake_minimum_required(VERSION 2.8.3)

# project name
project(archinfo)

# set C compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

# set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# archinfo executable file
add_executable(archinfo archinfo.c)

