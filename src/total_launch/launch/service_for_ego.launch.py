"""
Total System Launch File for Drone

This launch file starts all components required for the complete drone system:
1. Livox MID360 LiDAR Driver (external workspace)
2. DCL-SLAM (LiDAR odometry and mapping)
3. Drone Control System (via demo3.launch.py)
   - UART to STM32 bridge (serial communication)
   - Position PID controller (flight control)
   - Route target publisher (waypoint navigation)

Launch Order:
  T+0.0s: Livox Driver
  T+0.5s: DCL-SLAM
  T+2.0s: Drone Control System (demo3)

Usage:
  ros2 launch total_launch total_system.launch.py
  ros2 launch total_launch total_system.launch.py use_rviz:=true
  ros2 launch total_launch total_system.launch.py enable_livox_driver:=false
"""

import os
from pathlib import Path
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # ==================== 参数声明 ====================
    namespace = LaunchConfiguration("namespace", default="a")
    drone_id = LaunchConfiguration("drone_id", default="0")
    cloud_topic = LaunchConfiguration('cloud_topic', default='cloud_registered')
    odom_topic = LaunchConfiguration('odom_topic', default='Odometry')
    cloud_topic_cmd = DeclareLaunchArgument('cloud_topic', default_value=cloud_topic, description='PointCloud2 topic')
    odom_topic_cmd = DeclareLaunchArgument('odom_topic', default_value=odom_topic, description='Odometry topic')
    namespace_arg = DeclareLaunchArgument(
        "namespace",
        default_value=namespace,
        description="Namespace for the drone"
    )
    drone_id_arg = DeclareLaunchArgument(
        "drone_id",
        default_value=drone_id,
        description="Drone ID"
    )
    use_rviz_arg = DeclareLaunchArgument(
        "use_rviz",
        default_value="false",
        description="Launch RViz for visualization"
    )

    livox_launch_path_arg = DeclareLaunchArgument(
        "livox_launch_path",
        default_value="/home/intelcup/ws_livox/src/livox_ros_driver2/launch_ROS2/msg_MID360s_launch.py",
        description="Absolute path to Livox driver launch file"
    )
    
    # ==================== 包路径查找（方案 A：FindPackageShare + 回退）====================
    try:
        dcl_slam_share = FindPackageShare(package="dcl_slam").find("dcl_slam")
    except KeyError:
        dcl_slam_share = "/home/intelcup/ws_drone/src/dcl_slam"

    try:
        uart_to_stm32_pkg_share = FindPackageShare(package="uart_to_stm32").find("uart_to_stm32")
    except KeyError:
        uart_to_stm32_pkg_share = "/home/intelcup/ws_drone/src/uart_to_stm32"
    
    # ==================== 子系统启动文件 ====================
    # 1. Livox 驱动（立即启动 T+0.0s）
    livox_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            LaunchConfiguration("livox_launch_path")
        ),
        launch_arguments={
            "namespace": namespace,
            "drone_id": drone_id,
        }.items()
    )
    
    # 2. DCL-SLAM (延迟 0.5 秒启动 T+0.5s)
    dcl_slam_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(dcl_slam_share, "launch", "dcl_fast_lio_mid360s.launch.py")
        ),
        launch_arguments={
            "namespace": namespace,
            "drone_id": drone_id,
            "cloud_topic": cloud_topic,
            "odom_topic": odom_topic,
            "rviz": LaunchConfiguration("use_rviz"),
        }.items()
    )

    # 3. Uart to STM32 Bridge (延迟 1 秒启动 T+1.0s)
    uart_to_stm32_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(uart_to_stm32_pkg_share, "launch", "uart_to_stm32.launch.py")
        ),
        launch_arguments={
            "namespace": namespace,
            "drone_id": drone_id,
        }.items()
    )
    
    # ==================== 返回 Launch 描述 ====================
    return LaunchDescription([
        # 参数声明
        use_rviz_arg,
        livox_launch_path_arg,
        namespace_arg,
        drone_id_arg,
        cloud_topic_cmd,
        odom_topic_cmd,
        # 系统启动（带延迟）
        livox_launch,  # T+0.0s
        TimerAction(period=0.5, actions=[dcl_slam_launch]),  # T+0.5s
        TimerAction(period=1.0, actions=[uart_to_stm32_launch]),  # T+1.0s
    ])
