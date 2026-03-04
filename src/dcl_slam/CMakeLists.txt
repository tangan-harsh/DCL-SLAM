cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0074 NEW)
project(dcl_slam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(rclcpp REQUIRED)

find_package(PCL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)
find_package(GTSAM REQUIRED)
find_package(distributed_mapper REQUIRED)
find_package(libnabo REQUIRED)
find_package(glog REQUIRED)

set(msg_files
  msg/LoopInfo.msg
  msg/GlobalDescriptor.msg
  msg/NeighborEstimate.msg
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES std_msgs sensor_msgs geometry_msgs nav_msgs
)

ament_export_dependencies(rosidl_default_runtime)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${libnabo_INCLUDE_DIRS}
  ${GTSAM_INCLUDE_DIRS}
  ${distributed_mapper_INCLUDE_DIRS}
  ${pcl_conversions_INCLUDE_DIRS}
  ${glog_INCLUDE_DIRS}
)

add_library(dcl_slam_descriptors
  src/scanContextDescriptor.cpp
  src/lidarIrisDescriptor.cpp
  src/m2dpDescriptor.cpp
)
target_link_libraries(dcl_slam_descriptors
  ${PCL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${OpenMP_CXX_FLAGS}
  ${libnabo_LIBRARIES}
)
ament_target_dependencies(dcl_slam_descriptors
  rclcpp
)
# Ensure msgs are generated before compiling lib
rosidl_target_interfaces(dcl_slam_descriptors ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(dcl_slam_loopVisualizationNode
  src/loopVisualizationNode.cpp
)
target_link_libraries(dcl_slam_loopVisualizationNode
  ${PCL_LIBRARIES}
  gtsam
  glog::glog
)
ament_target_dependencies(dcl_slam_loopVisualizationNode
  rclcpp
  pcl_conversions
  visualization_msgs
  geometry_msgs
  std_msgs
)
rosidl_target_interfaces(dcl_slam_loopVisualizationNode ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_library(dcl_slam_core SHARED
  src/paramsServer.cpp
  src/distributedMappingBasis.cpp
  src/distributedMapping.cpp
  src/distributedLoopClosure.cpp
  src/distributedMappingVisualization.cpp
)

target_link_libraries(dcl_slam_core
  dcl_slam_descriptors
  ${PCL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${OpenMP_CXX_FLAGS}
  gtsam
  distributed_mapper::distributed_mapper
  glog::glog
)
ament_target_dependencies(dcl_slam_core
  rclcpp
  pcl_conversions
  tf2
  tf2_ros
  tf2_geometry_msgs
  visualization_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  std_msgs
)
rosidl_target_interfaces(dcl_slam_core ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(dcl_slam_node
  src/dcl_slam_node.cpp
)

target_link_libraries(dcl_slam_node
  dcl_slam_core
)

rosidl_target_interfaces(dcl_slam_node ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS dcl_slam_node dcl_slam_loopVisualizationNode
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

ament_export_include_directories(include/${PROJECT_NAME})
ament_export_libraries(dcl_slam_core dcl_slam_descriptors)
ament_export_targets(dcl_slam_targets HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp
  pcl_conversions
  tf2
  tf2_ros
  tf2_geometry_msgs
  visualization_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  std_msgs
)

install(TARGETS dcl_slam_core dcl_slam_descriptors
  EXPORT dcl_slam_targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/${PROJECT_NAME}
)

ament_package()