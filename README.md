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
    ├── fwr_cpp/           # C++ based nodes (Vision, Control, etc.)
    ├── fwr_interfaces/    # Custom ROS 2 Messages (msg) and Services (srv)
    └── fwr_python/        # Python based nodes (Logic, Decision making)