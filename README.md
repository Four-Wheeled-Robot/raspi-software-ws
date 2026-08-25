# 🤖 ROS 2 Robot Workspace

This repository contains the workspace, in which we will develop the software for Raspberry Pi on our four wheeled robot. It is designed to be fully automated and easily deployable across different environments, including local development machines (OrbStack/Docker) and Raspberry Pi hardware.

## 📋 Prerequisites

Before starting, ensure you have the following on your system:
- **OS**: Ubuntu 24.04.4 LTS (or compatible)
- **ROS**: Lyrical Luth (or your specific ROS 2 distribution)
- **Git**: Installed system-wide

## 📂 Workspace Structure

The workspace is organized into a standard ROS 2 layout. The primary development happens inside the `src` directory, which is divided into four distinct packages:

```text
raspi-software-ws/
└── src/                   # Source code for the workspace
    ├── fwr_bringup/       # Launch files and global configurations
    ├── fwr_interfaces/    # Custom ROS 2 Messages (msg) and Services (srv)
    ├── fwr_teleop/        # Keyboard teleoperation node for Holonomic/Differential control
    └── fwr_controller/    # Operation packages (Motor drivers, hardware interfaces)
```

## 🚀 Installation and Build

1. **Clone the repository:**
   ```bash
   git clone <your_repository_url> ~/raspi-software-ws
   cd ~/raspi-software-ws
   ```

2. **Build the workspace:**
   Using `--symlink-install` allows you to modify Python scripts and YAML files without needing to rebuild every time.
   ```bash
   colcon build --symlink-install
   ```

3. **Source the environment:**
   ```bash
   source install/setup.bash
   ```
   *(Tip: Run `echo "source ~/raspi-software-ws/install/setup.bash" >> ~/.bashrc` to auto-load the workspace in every new terminal).*

## 🎮 Usage

For standard operation, especially via SSH to the Raspberry Pi, it is recommended to use two separate terminal sessions.

### 1. Launch Hardware & Background Nodes
In **Terminal 1**, start the main robot bringup launch file. This will initialize motor drivers, sensors, and state publishers once they are added to `fwr.launch.py`:
```bash
ros2 launch fwr_bringup fwr.launch.py
```

### 2. Run Teleoperation
In **Terminal 2**, start the keyboard controller to drive the robot. This node automatically loads default parameters (like speed and turn rate) from the `fwr_bringup` configuration:
```bash
ros2 run fwr_teleop teleop_controller --ros-args --params-file src/fwr_bringup/config/teleop_params.yaml
```

### ⌨️ Teleop Controls Reference
- **Normal Drive**: `i` (Forward), `,` (Backward), `j` / `l` (Rotate)
- **Holonomic / Strafing (Omni-wheels)**: `Shift + J` (Strafe Left), `Shift + L` (Strafe Right)
- **Stop**: `k` or Spacebar
- **Speed Control**: `q` / `z` (Increase / Decrease max speed by 10%)