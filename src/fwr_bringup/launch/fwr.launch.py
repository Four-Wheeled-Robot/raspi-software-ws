import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    # 1. Path to the config directory in fwr_bringup (shared across future nodes)
    # config_dir = os.path.join(get_package_share_directory('fwr_bringup'), 'config')

    # 2. Declare the Arduino / ESP32 hardware communication node (fwr_controller)
    # motor_node = Node(
    #     package='fwr_controller',
    #     executable='motor_driver',
    #     name='motor_driver_node',
    #     output='screen'
    # )

    # 3. Declare the LiDAR / IMU sensor node (Example)
    # lidar_node = Node(
    #     package='sllidar_ros2',
    #     executable='sllidar_node',
    #     name='sllidar_node',
    #     output='screen'
    # )

    return LaunchDescription([
        # motor_node,
        # lidar_node,
    ])