import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch_ros.actions import Node


def generate_launch_description():
    dcl_slam_dir = get_package_share_directory('dcl_slam')
    config_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.yaml')
    rviz_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.rviz')

    rviz_use = LaunchConfiguration('rviz')

    declare_rviz_cmd = DeclareLaunchArgument(
        'rviz', default_value='true',
        description='Launch RViz'
    )

    fastlio_node = Node(
        package='dcl_fast_lio',
        executable='fastlio_mapping',
        name='laserMapping',
        namespace='/a',
        parameters=[config_path],
        output='screen',
        remappings=[
            ('/a/livox/imu', '/livox/imu'),
            ('/a/livox/lidar', '/livox/lidar'),
        ]
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', rviz_path],
        condition=IfCondition(rviz_use)
    )

    ld = LaunchDescription()
    ld.add_action(declare_rviz_cmd)
    ld.add_action(fastlio_node)
    ld.add_action(rviz_node)

    return ld
