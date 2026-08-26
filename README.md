# 🤖 ROS 2 Four-Wheeled Robot Workspace

This repository contains the software workspace for a four-wheeled robot (supporting Differential and Mecanum/Omni Holonomic drive) running on a Raspberry Pi. It is designed to be highly optimized for real-time embedded systems, modular, and configurable via YAML parameters.

## 📋 Prerequisites

Before starting, ensure you have the following on your system:
- **OS**: Ubuntu 24.04 LTS (or compatible)
- **ROS 2**: Jazzy Jalisco / Humble Hawksbill
- **Build Tools**: `colcon` with symlink support (`sudo apt install python3-colcon-common-extensions`)
- **Git**: Installed system-wide

## 📂 Workspace Structure

The workspace strictly separates hardware abstraction, kinematics computing, user inputs, and launch orchestration:

```text
raspi-software-ws/
└── src/                   
    ├── fwr_bringup/       # Orchestrator: Launch files & YAML parameter configs (ament_cmake)
    │   ├── config/        # Global parameters (teleop_params.yaml, controller_params.yaml)
    │   └── launch/        # System launch files (fwr.launch.py)
    ├── fwr_controller/    # C++ Kinematics Node (Twist -> 4-Wheel RPM)
    ├── fwr_interfaces/    # Custom ROS 2 Messages (msg), Services (srv), and Actions
    ├── fwr_teleop/        # Python keyboard teleoperation node (Twist publisher)
    └── ....               # WIP
```

## 📐 Kinematics & Control Engine (Optimized C++)

The `fwr_controller` node is written in modern C++ and strictly optimized for **Real-time Embedded Systems (Raspberry Pi / ARM64)**:
- **Zero Dynamic Allocation:** `std::array` and pre-allocated message buffers ensure no `malloc/new` occurs during runtime callbacks, eliminating CPU overhead and heap fragmentation.
- **Fused Pre-computation:** Kinematic scale factors and hardware direction multipliers are pre-computed at startup.
- **Inverse Kinematics:** Converts `geometry_msgs/Twist` (`/cmd_vel`) into a `std_msgs/Float64MultiArray` (`/wheel_speed_cmd`) containing `[FL, FR, RL, RR]` RPM values.

$$\omega_{FL} = \frac{1}{r} \left( v_x - v_y - (L_x + L_y)\omega_z \right) \times dir_{FL}$$
$$\omega_{FR} = \frac{1}{r} \left( v_x + v_y + (L_x + L_y)\omega_z \right) \times dir_{FR}$$
$$\omega_{RL} = \frac{1}{r} \left( v_x + v_y - (L_x + L_y)\omega_z \right) \times dir_{RL}$$
$$\omega_{RR} = \frac{1}{r} \left( v_x - v_y + (L_x + L_y)\omega_z \right) \times dir_{RR}$$

## 🕹️ Keyboard Teleoperation (Python)

The `fwr_teleop` package provides a robust keyboard interface to control the robot manually. Written in Python for easy terminal I/O handling, it features:
- **Dual Drive Modes:** Supports standard differential driving and full Holonomic/Mecanum strafing (lateral/diagonal movement).
- **Dynamic Speed Adjustment:** Change linear and angular velocities on-the-fly using predefined speed steps.
- **Twist & TwistStamped:** Configurable to publish either standard `geometry_msgs/Twist` or time-stamped `TwistStamped` messages based on your navigation stack requirements.

## ⚙️ Configuration (`fwr_bringup/config`)

All parameters are centralized in the `fwr_bringup` package to keep the source code clean. *Note: Topic names are deliberately excluded from YAML to follow ROS 2 Topic Remapping best practices.*

### 1. `controller_params.yaml` (Hardware & Kinematics)
```yaml
fwr_controller:
  ros__parameters:
    wheel_radius: 0.04          # Wheel radius in meters (40mm)
    wheel_base_x: 0.125         # Distance from center to front/rear axle (125mm)
    wheel_base_y: 0.150         # Distance from center to left/right wheel (150mm)
    max_wheel_rpm: 300.0        # Motor hardware limit

    # Hardware Direction Multiplier [FL, FR, RL, RR]
    # Change 1.0 to -1.0 to easily invert a motor's direction without rewiring hardware.
    wheel_direction: [1.0, 1.0, 1.0, 1.0]
```

### 2. `teleop_params.yaml` (Teleoperation Settings)
```yaml
teleop_controller:
  ros__parameters:
    speed: 0.5           # Initial linear speed (m/s)
    turn: 1.0            # Initial angular speed (rad/s)
    speed_step: 0.1      # Increment step for linear speed
    turn_step: 0.2       # Increment step for angular speed
    stamped: false       # Use TwistStamped instead of Twist if true
```

## 🚀 Installation and Build (Work In Progress)

1. **Clone the repository:**
   ```bash
   git clone <your_repository_url> ~/raspi-software-ws
   cd ~/raspi-software-ws
   ```

2. **Install dependencies:**
   ```bash
   rosdep update
   rosdep install --from-paths src -y --ignore-src
   ```

3. **Build the workspace:**
   ```bash
   colcon build --symlink-install
   ```

4. **Source the workspace environment:**
   ```bash
   source install/setup.bash
   ```

## 🎮 Usage (Work In Progress)

For operation via SSH to the Raspberry Pi , use separate terminal sessions (or `tmux`):

### 1. Launch the Robot System
In **Terminal 1**, launch the controller node with YAML parameters applied automatically:
```bash
ros2 launch fwr_bringup fwr.launch.py
```
*(Optional)* If you want to run multiple robots or change topic names, use standard ROS 2 remapping:
```bash
ros2 launch fwr_bringup fwr.launch.py cmd_vel:=/robot1/cmd_vel
```

### 2. Run Keyboard Teleoperation
In **Terminal 2**, start the interactive keyboard controller:
```bash
ros2 run fwr_teleop teleop_controller --ros-args --params-file src/fwr_bringup/config/teleop_params.yaml
```

### 3. Monitor Hardware Commands
In **Terminal 3**, observe the real-time RPM arrays sent to the hardware layer (Arduino/ESP32/Motor Drivers):
```bash
ros2 topic echo /wheel_speed_cmd
```

---

### ⌨️ Teleop Controls Reference

| Action | Standard Mode (Differential) | Holonomic Mode (Omni / Mecanum) |
| :--- | :--- | :--- |
| **Move / Translate** | `i` (Forward) / `,` (Backward) | `Shift + I` / `Shift + <` |
| **Strafe (Lateral)** | *N/A* | `Shift + J` (Left) / `Shift + L` (Right) |
| **Diagonal** | *N/A* | `Shift + U` / `Shift + O` / `Shift + M` / `Shift + >` |
| **Rotate** | `j` (Turn Left) / `l` (Turn Right) | `u` / `o` (Turn while moving) |
| **Stop** | `k` or `Spacebar` | `k` or `Spacebar` |
| **Speed Adjustment** | `q` / `z` (Increase / Decrease linear speed) | `w` / `x` (Angular speed) |