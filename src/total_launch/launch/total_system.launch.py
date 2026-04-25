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
  ros2 launch total_launch total_system.launch.py use_rviz:=false
  ros2 launch total_launch total_system.launch.py enable_drone_system:=false
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
    use_rviz_arg = DeclareLaunchArgument(
        "use_rviz",
        default_value="true",
        description="Launch RViz for visualization"
    )
    
    enable_drone_system_arg = DeclareLaunchArgument(
        "enable_drone_system",
        default_value="true",
        description="Enable drone control system (demo3)"
    )
    
    enable_livox_driver_arg = DeclareLaunchArgument(
        "enable_livox_driver",
        default_value="true",
        description="Enable Livox MID360 LiDAR driver"
    )
    
    livox_launch_path_arg = DeclareLaunchArgument(
        "livox_launch_path",
        default_value="/home/intelcup/ws_livox/src/livox_ros_driver2/launch_ROS2/msg_MID360_launch.py",
        description="Absolute path to Livox driver launch file"
    )
    
    # ==================== 包路径查找（方案 A：FindPackageShare + 回退）====================
    try:
        dcl_slam_share = FindPackageShare(package="dcl_slam").find("dcl_slam")
        my_launch_share = FindPackageShare(package="my_launch").find("my_launch")
    except KeyError:
        dcl_slam_share = "/home/intelcup/ws_drone/src/dcl_slam"
        my_launch_share = "/home/intelcup/ws_drone/src/Drone_ROS2/my_launch"
    
    # ==================== 子系统启动文件 ====================
    # 1. Livox 驱动（立即启动 T+0.0s）
    livox_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            LaunchConfiguration("livox_launch_path")
        ),
        condition=IfCondition(LaunchConfiguration("enable_livox_driver"))
    )
    
    # 2. DCL-SLAM (延迟 0.5 秒启动 T+0.5s)
    dcl_slam_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(dcl_slam_share, "launch", "dcl_fast_lio_mid360.launch.py")
        ),
        launch_arguments={
            "rviz": LaunchConfiguration("use_rviz"),
        }.items()
    )
    
    # 3. Drone 系统 (延迟 2 秒启动 T+2.0s)
    demo3_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(my_launch_share, "launch", "demo3.launch.py")
        ),
        condition=IfCondition(LaunchConfiguration("enable_drone_system"))
    )
    
    # ==================== 返回 Launch 描述 ====================
    return LaunchDescription([
        # 参数声明
        use_rviz_arg,
        enable_drone_system_arg,
        enable_livox_driver_arg,
        livox_launch_path_arg,
        
        # 系统启动（带延迟）
        livox_launch,  # T+0.0s
        TimerAction(period=0.5, actions=[dcl_slam_launch]),  # T+0.5s
        TimerAction(period=2.0, actions=[demo3_launch]),  # T+2.0s
    ])
