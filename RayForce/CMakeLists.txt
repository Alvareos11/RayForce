cmake_minimum_required(VERSION 3.10)
project(RayForce)

# 1. BUILD CONFIGURATION
# Default to Release mode if no build type is specified for maximum performance
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization Flags for Linux (x86 and ARM compatible)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # -O3 and -ffast-math provide high-performance math optimizations
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")
    add_definitions(-DNDEBUG)
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    add_definitions(-D_DEBUG)
endif()

# Disable CUDA if building for non-NVIDIA systems or general Linux compatibility
add_definitions(-DDONT_USE_CUDA)

# 2. DEPENDENCY DISCOVERY
# Attempt to find Raylib via the system's CMake package registry
find_package(raylib 4.0 QUIET) 

# 3. PHYSX PATH CONFIGURATION
# Allows users to pass a custom path via: cmake -DPHYSX_PATH=/custom/path ..
if(NOT PHYSX_PATH)
    set(PHYSX_PATH "/usr/local")
endif()

set(PHYSX_INCLUDE_DIR "${PHYSX_PATH}/include")
set(PHYSX_LIB_DIR "${PHYSX_PATH}/lib")

# 4. SOURCE CODE DISCOVERY
# Recursively find all C++ files within the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 5. EXECUTABLE DEFINITION
add_executable(${PROJECT_NAME} ${SOURCES})

# 6. INCLUDE DIRECTORIES
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PHYSX_INCLUDE_DIR}
    "${PHYSX_INCLUDE_DIR}/physx"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# 7. LIBRARY LINKING (Linux Structure)
# Note: Link order for PhysX is critical for GCC/Clang to resolve symbols correctly.
target_link_libraries(${PROJECT_NAME}
    # System Dependencies
    m          # Math library
    pthread    # Multithreading support
    dl         # Dynamic linking
    rt         # Real-time extensions
    GL         # OpenGL
    X11        # Windowing system
    
    # Visual Framework
    raylib

    # NVIDIA PhysX Static Libraries
    "${PHYSX_LIB_DIR}/libPhysXExtensions_static_64.a"
    "${PHYSX_LIB_DIR}/libPhysXCooking_static_64.a"
    "${PHYSX_LIB_DIR}/libPhysX_static_64.a"
    "${PHYSX_LIB_DIR}/libPhysXCommon_static_64.a"
    "${PHYSX_LIB_DIR}/libPhysXFoundation_static_64.a"
)