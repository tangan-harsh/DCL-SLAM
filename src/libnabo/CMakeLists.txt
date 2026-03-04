cmake_minimum_required(VERSION 3.5)
project(libnabo)

# Check if we are running in a ROS environment
find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
  nabo
  ${EIGEN3_INCLUDE_DIRS}
)

# Add library
add_library(libnabo
nabo/nabo.cpp
nabo/brute_force_cpu.cpp
nabo/kdtree_cpu.cpp
nabo/kdtree_opencl.cpp
)

# Export include directories and libraries
ament_export_include_directories(.)
ament_export_libraries(libnabo)

# Install
install(
  TARGETS libnabo
  EXPORT export_libnabo
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY nabo
  DESTINATION include
)

ament_package()
