import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, OpaqueFunction
from launch.conditions import IfCondition, LaunchConfigurationEquals
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, PushRosNamespace

def launch_setup(context, *args, **kwargs):
    dcl_slam_dir = get_package_share_directory('dcl_slam')
    robot_prefix = LaunchConfiguration('robotPrefix').perform(context)
    respawn_value = LaunchConfiguration('respawnValue')
    
    # Construct config file path
    # e.g., config/dcl_lio_sam_vlp16_params_a.yaml
    config_file_name = f'dcl_lio_sam_vlp16_params_{robot_prefix}.yaml'
    config_file_path = os.path.join(dcl_slam_dir, 'config', config_file_name)
    
    # Note: We are not checking if file exists here, assuming user configuration is correct.

    # Group for LIO-SAM (lioType == '1')
    lio_sam_group = GroupAction(
        condition=LaunchConfigurationEquals('lioType', '1'),
        actions=[
            PushRosNamespace(robot_prefix),
            
            # Note: The original launch file used <rosparam command="load"> which loads params into the namespace.
            # In ROS2, we usually pass parameters directly to nodes.
            # Since we don't know the exact nodes of dcl_lio_sam (it's external),
            # we will assume we pass the config file to the known nodes we invoke.
            # OR we can attempt to load it globally for the namespace (less common in ROS2 but possible via 'parameters' list).
            
            # Nodes
            Node(
                package='dcl_lio_sam',
                executable='dcl_lio_sam_imuPreintegration',
                name='dcl_lio_sam_imuPreintegration',
                output='screen',
                respawn=True, # simplified
                parameters=[config_file_path]
            ),
            Node(
                package='dcl_lio_sam',
                executable='dcl_lio_sam_imageProjection',
                name='dcl_lio_sam_imageProjection',
                output='screen',
                respawn=True,
                parameters=[config_file_path]
            ),
            Node(
                package='dcl_lio_sam',
                executable='dcl_lio_sam_featureExtraction',
                name='dcl_lio_sam_featureExtraction',
                output='screen',
                respawn=True,
                parameters=[config_file_path]
            ),
            Node(
                package='dcl_lio_sam',
                executable='dcl_lio_sam_mapOptmization',
                name='dcl_lio_sam_mapOptmization',
                output='screen',
                respawn=True,
                parameters=[config_file_path]
            ),
            
            # DCL-SLAM Distributed Mapping Node
            Node(
                package='dcl_slam',
                executable='dcl_slam_node',
                name='dcl_slam_node',
                output='screen',
                parameters=[config_file_path]
            ),
            
            # Robot State Publisher
            Node(
                package='robot_state_publisher',
                executable='robot_state_publisher',
                name='robot_state_publisher',
                parameters=[{'tf_prefix': robot_prefix}],
                arguments=[os.path.join(dcl_slam_dir, 'config', 'lio_sam_robot.urdf.xacro')]
            )
        ]
    )
    
    return [lio_sam_group]

def generate_launch_description():
    declare_lio_type = DeclareLaunchArgument('lioType', default_value='1')
    declare_robot_prefix = DeclareLaunchArgument('robotPrefix', default_value='a')
    declare_respawn_value = DeclareLaunchArgument('respawnValue', default_value='false')
    
    return LaunchDescription([
        declare_lio_type,
        declare_robot_prefix,
        declare_respawn_value,
        OpaqueFunction(function=launch_setup)
    ])
