cmake_minimum_required(VERSION 3.8)
project(distributed_mapper)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ament_cmake REQUIRED)
find_package(GTSAM REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
  include
  include/pairwise_consistency_maximization
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/
  ${GTSAM_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)

# Graph utils library
add_library(graph_utils src/pairwise_consistency_maximization/graph_utils/graph_utils_functions.cpp)
target_link_libraries(graph_utils gtsam)

# Robot local map library
add_library(robot_measurements src/pairwise_consistency_maximization/robot_measurements/robot_measurements.cpp src/pairwise_consistency_maximization/robot_measurements/interrobot_measurements.cpp src/pairwise_consistency_maximization/robot_measurements/robot_local_map.cpp)
target_link_libraries(robot_measurements graph_utils gtsam)

# Paiwise Consistency computation library
add_library(pairwise_consistency src/pairwise_consistency_maximization/pairwise_consistency/pairwise_consistency.cpp)
target_link_libraries(pairwise_consistency robot_measurements gtsam)

# Maximum Clique Solver
add_library(fast_max-clique_finder src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/findClique.h src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/graphIO.h src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/findClique.cpp src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/findCliqueHeu.cpp  src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/utils.cpp  src/pairwise_consistency_maximization/third_parties/fast_max-clique_finder/src/graphIO.cpp
)
target_compile_options(fast_max-clique_finder PRIVATE -w)

# Global map solver library
add_library(global_map_solver src/pairwise_consistency_maximization/global_map/global_map.cpp)
target_link_libraries(global_map_solver graph_utils pairwise_consistency fast_max-clique_finder gtsam)

# Distributed PCM library
add_library(distributed_pcm src/pairwise_consistency_maximization/distributed_pcm/distributed_pcm.cpp)
target_link_libraries(distributed_pcm global_map_solver gtsam)

# Distributed Mapper
add_library(distributed_mapper src/distributed_mapper/distributed_mapper.cpp src/distributed_mapper/evaluation_utils.cpp include/distributed_mapper/distributed_mapper_utils.h src/distributed_mapper/distributed_mapper_utils.cpp src/distributed_mapper/run_distributed_mapper.cpp)
target_link_libraries(distributed_mapper graph_utils robot_measurements pairwise_consistency fast_max-clique_finder distributed_pcm global_map_solver gtsam)

ament_export_include_directories(include)
ament_export_libraries(distributed_mapper)

install(
  TARGETS distributed_mapper graph_utils robot_measurements pairwise_consistency fast_max-clique_finder distributed_pcm global_map_solver
  EXPORT export_distributed_mapper
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_export_targets(export_distributed_mapper)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_package()
