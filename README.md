# RayForce

## Description
Rayforce is a lightweight 3d physics game engine, with the purpose of having the maximum performance.

---
## Requirements & Dependencies

### System Requirements
* **CMake:** Version 3.15 or higher
* **C++17 Compiler:** MSVC (Visual Studio), GCC, or Clang
* **NVIDIA PhysX SDK:** Must be pre-installed on your system

### Core Libraries
* **Raylib:** Used for rendering and window management (automatically fetched by CMake).
* **NVIDIA PhysX:** Utilized for high-performance physics simulations (must be installed separately).

---
## Installation

To get a local copy up and running, follow these steps:

### 1. Clone the Repository
```bash
git clone https://github.com/Alvareos11/RayForce.git
cd RayForce
```

### 2. Install External Dependencies

#### Install NVIDIA PhysX SDK
**Windows (MSVC):**
- Download the NVIDIA PhysX SDK from the [official repository](https://github.com/NVIDIA-Omniverse/PhysX)
- Follow the [PhysX Quick Start Guide](https://github.com/NVIDIA-Omniverse/PhysX#quick-start) for Windows
- Install to a location (default: `C:\Program Files\NVIDIA PhysX SDK` or `/usr/local` on Linux/macOS)
- Set the environment variable: `PHYSX_PATH=<your_physx_installation_path>`

**Linux/macOS:**
```bash
# Refer to NVIDIA PhysX official documentation for your OS
# After installation, set:
export PHYSX_PATH=/usr/local  # or your custom installation path
```

#### Install CMake and C++ Compiler
**Windows:**
- Download CMake from https://cmake.org/download/
- Visual Studio 2022 with C++17 or higher

**Linux:**
```bash
sudo apt-get install cmake build-essential
```

**macOS:**
```bash
brew install cmake
```

### 3. Build the Project

Using CMake from the root directory:
```bash
cd RayForce
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Alternatively, on Linux/macOS with Make:
```bash
cd RayForce
mkdir build
cd build
cmake ..
make
```

---
### Troubleshooting

**PhysX Library Not Found:**
- Ensure PhysX SDK is properly installed
- Set the `PHYSX_PATH` environment variable before running CMake if you don`t use any packet manager:
  ```bash
  # Windows
  set PHYSX_PATH=C:\path\to\physx
  
  # Linux/macOS
  export PHYSX_PATH=/path/to/physx
  ```
- Then reconfigure CMake: `cmake ..`

**On Linux / Raspberry Pi:**
If the engine fails to start, force the OpenGL version by running:
```bash
MESA_GL_VERSION_OVERRIDE=3.3 ./physics_demo
```

## Acknowledgments and Legal Notices

**RayForce** is an open-source project that integrates third-party technologies. By using this software, you acknowledge and agree to the license terms of the following components:

### NVIDIA® PhysX® SDK
This project uses the **NVIDIA® PhysX® SDK**.
- Copyright © 2026 NVIDIA Corporation. All rights reserved.
- Licensed under the **BSD-3-Clause License**.
- NVIDIA® and PhysX® are registered trademarks of NVIDIA Corporation.

### raylib
This project uses **raylib** for window management and rendering.
- Copyright © 2013-2026 Ramon Santamaria (@raysan5).
- Licensed under the **zlib/libpng License**.

## License
**RayForce** is licensed under the **GNU General Public License v3.0 (GPL-3.0)**. 
Copyright © 2026 [Your Name].

### glTF Sample Models
This project uses 3D models from the [glTF Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models) repository by Khronos Group.
- These models are used for testing and demonstration purposes.
- Licensed under [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/).
