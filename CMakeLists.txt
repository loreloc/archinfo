
# minimum cmake version
cmake_minimum_required(VERSION 2.8.3)

# project name
project(archinfo)

# set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# archinfo executable file
add_executable(archinfo archinfo.c)

