# The version number.
set (STURDY_ROBOT_VERSION_MAJOR 0)
set (STURDY_ROBOT_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "sturdy-robot-config.hpp.in"
  "${PROJECT_BINARY_DIR}/InpOpts/sturdy-robot-config.hpp"
  )

include_directories("${PROJECT_BINARY_DIR}/InpOpts/")

add_library (InpOpts InpOpts.cpp InpOptsClass.cpp)
add_definitions(-std=c++11)