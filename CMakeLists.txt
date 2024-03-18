cmake_minimum_required(VERSION 3.13)


# give a name to the project
project(Worms)

# (optaional) put the binary files in this directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# add lib c3ga
message(STATUS "c3ga garamon cmake part ..." )
add_subdirectory(lib/garamon_c3ga INTERFACE)
include_directories(${CMAKE_SOURCE_DIR}/lib/garamon_c3ga/src/) # important : other projects called by this cmake will know this include directory

# add tp code
message(STATUS "Worms cmake part ..." )
add_subdirectory(src)

