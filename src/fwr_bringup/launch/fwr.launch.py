import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # Get the path to the YAML config file
    controller_params = os.path.join(
        get_package_share_directory('fwr_bringup'),
        'config',
        'controller_params.yaml'
    )

    # Declare the Node and pass parameters
    controller_node = Node(
        package='fwr_controller',
        executable='mecanum_controller',
        name='fwr_controller',          # Must match the root key in the YAML file
        parameters=[controller_params],  # Load the YAML file here
        output='screen'
    )

    return LaunchDescription([
        controller_node
    ])