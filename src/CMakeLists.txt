cmake_minimum_required(VERSION 3.13)

project(ga_view)

# set the module path for "findLib.cmake"
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

# require Eigen
find_package(Eigen3 REQUIRED)
if(${EIGEN3_FOUND})
    message(STATUS "lib EIGEN3 found")
    message(STATUS "  version " ${EIGEN3_VERSION_STRING})
    message(STATUS "  include " ${EIGEN3_INCLUDE_DIR})
endif()

# require SDL2
# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})


# compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# files to compile
file(GLOB_RECURSE source_files src/*.cpp src/*.hpp)
message(STATUS "  files " ${source_files})

# compile
add_executable(ga_view ${source_files})
#add_executable(ga_view src/Directory.cpp src/Directory.hpp src/Entry.hpp src/Entry.cpp src/main.cpp )

# includes
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${C3GA_INCLUDE_DIRS})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

# link
# target_link_libraries(ga_view ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(ga_view ${C3GA_LIBRARIES})




