import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, GroupAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, EnvironmentVariable
from launch_ros.actions import Node

def generate_launch_description():
    # Get the package directory
    dcl_slam_dir = get_package_share_directory('dcl_slam')
    
    # Arguments
    set_lio_type = LaunchConfiguration('set_lio_type')
    set_respawn_value = LaunchConfiguration('set_respawn_value')
    
    declare_lio_type = DeclareLaunchArgument(
        'set_lio_type',
        default_value='1',
        description='1 for LIO-SAM, 2 for FAST-LIO2'
    )
    
    declare_respawn_value = DeclareLaunchArgument(
        'set_respawn_value',
        default_value='false',
        description='Respawn nodes if they crash'
    )

    # Global Parameters
    # Note: In ROS2, global parameters are less common. We usually pass them to nodes.
    # But for compatibility, we can use a shared yaml or pass to each node.
    # Here we assume single_ugv.launch.py handles passing params to its nodes.

    # Nodes
    rviz_config = os.path.join(dcl_slam_dir, 'config', 'dcl_rviz.rviz')
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='dcl_rviz',
        arguments=['-d', rviz_config],
        output='screen'
    )
    
    loop_vis_node = Node(
        package='dcl_slam',
        executable='dcl_slam_loopVisualizationNode',
        name='dcl_slam_loopVisualizationNode',
        output='screen',
        parameters=[{'number_of_robots': 3}]
    )

    # Launch robots (a, b, c)
    # We need to convert single_ugv.launch to Python first, assuming it exists or we mock it.
    # Since single_ugv.launch is complex and calls other packages, we'll assume it's migrated too.
    
    launch_robot_a = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
        launch_arguments={
            'robotPrefix': 'a',
            'respawnValue': set_respawn_value,
            'lioType': set_lio_type
        }.items()
    )
    
    launch_robot_b = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
        launch_arguments={
            'robotPrefix': 'b',
            'respawnValue': set_respawn_value,
            'lioType': set_lio_type
        }.items()
    )
    
    launch_robot_c = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
        launch_arguments={
            'robotPrefix': 'c',
            'respawnValue': set_respawn_value,
            'lioType': set_lio_type
        }.items()
    )
    
    # Bag players
    # Using ros2bag play. Syntax is different from rosbag play.
    # We need to find the bag files.
    home_dir = EnvironmentVariable('HOME')
    
    # Example bag file path construction (adjust as needed)
    # bag_path = [home_dir, '/rosbag-data/S3E/SYSU_LIBRARY.bag'] 
    
    # Note: 'rosbag play' in ROS1 supports -p for prefix. ROS2 'ros2 bag play' does NOT support prefix remapping easily in the same way.
    # We usually use topic remapping.
    # This part requires careful manual adjustment by the user.
    
    return LaunchDescription([
        declare_lio_type,
        declare_respawn_value,
        rviz_node,
        loop_vis_node,
        launch_robot_a,
        launch_robot_b,
        launch_robot_c,
        # Bag players omitted as they require complex remapping setup in ROS2
    ])
