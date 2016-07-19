cmake_minimum_required (VERSION 2.6)
project (fantastic_train)

SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG          "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# The version number.
set (FANTASTIC_TRAIN_VERSION_MAJOR 0)
set (FANTASTIC_TRAIN_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/fantastic-train-config.hpp.in"
  "${PROJECT_BINARY_DIR}/fantastic-train-config.hpp"
  )

# add the binary tree to the search path for include files
# so that we will find configured header files
include_directories("${PROJECT_BINARY_DIR}")

# Add the directory for our input argument parsing library
set (INP_OPTS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/InpOpts/")
include_directories(${INP_OPTS_SOURCE_DIR})
add_subdirectory(InpOpts)
set (FANTASTIC_TRAIN_LIBS ${FANTASTIC_TRAIN_LIBS} InpOpts)

# add the binary tree to the search path for include files
# so that we will find configured header files
#include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/InpOpts/")

# Tell cmake to find Eigen and how to do that
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# We want to use some unsupported Eigen features, so tell cmake how to find the necessary files
include_directories(${EIGEN3_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR}/unsupported/)

# Add the executables
add_executable (fantastic_train main.cpp MatrixFileFunctions.cpp StringFunctions.cpp Modulators.cpp BEChannel.cpp BECDecoder.cpp FantasticTrainMainFunctions.cpp)
target_link_libraries (fantastic_train ${FANTASTIC_TRAIN_LIBS})

add_definitions(-std=c++11)

# Use the C++ '11 standard
set_property(TARGET fantastic_train PROPERTY CXX_STANDARD_REQUIRED 11)