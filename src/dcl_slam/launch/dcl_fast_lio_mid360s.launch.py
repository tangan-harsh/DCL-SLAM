import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch_ros.actions import Node


def generate_launch_description():

    drone_id = LaunchConfiguration('drone_id', default=0)
    cloud_topic = LaunchConfiguration('cloud_topic', default='cloud_registered')
    odom_topic = LaunchConfiguration('odom_topic', default='Odometry')
    namespace = LaunchConfiguration('namespace', default='a')
    
    drone_id_arg = DeclareLaunchArgument("drone_id",default_value=drone_id,description="Drone ID")
    cloud_topic_cmd = DeclareLaunchArgument('cloud_topic', default_value=cloud_topic, description='PointCloud2 topic')
    odom_topic_cmd = DeclareLaunchArgument('odom_topic', default_value=odom_topic, description='Odometry topic')
    namespace_cmd = DeclareLaunchArgument('namespace', default_value=namespace, description='Namespace')
    
    dcl_slam_dir = get_package_share_directory('dcl_slam')
    config_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360s.yaml')
    rviz_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360s.rviz')

    rviz_use = LaunchConfiguration('rviz')

    declare_rviz_cmd = DeclareLaunchArgument(
        'rviz', default_value='true',
        description='Launch RViz'
    )

    fastlio_node = Node(
        package='dcl_fast_lio',
        executable='fastlio_mapping',
        # name='laserMapping',
        namespace=namespace,
        parameters=[config_path],
        remappings=[
            ("cloud_registered", ['drone_', drone_id, '_', cloud_topic]),
            ("Odometry", ['drone_', drone_id, '_', odom_topic]),
            ('livox/lidar', ['drone_', drone_id, '_', 'livox/lidar']),
            ('livox/imu', ['drone_', drone_id, '_', 'livox/imu']),
            ('Odom_high_fre',['drone_', drone_id, '_', 'Odom_high_fre'])
        ],
        output='screen'
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', rviz_path],
        condition=IfCondition(rviz_use)
    )

    ld = LaunchDescription()

    ld.add_action(drone_id_arg)
    ld.add_action(cloud_topic_cmd)
    ld.add_action(odom_topic_cmd)
    ld.add_action(namespace_cmd)

    ld.add_action(declare_rviz_cmd)
    ld.add_action(fastlio_node)
    ld.add_action(rviz_node)

    return ld
