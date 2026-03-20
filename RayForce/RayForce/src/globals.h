#pragma once

#include <raylib.h>
#include <raymath.h>

#include <PxPhysicsAPI.h>

#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxExtensionsAPI.h>

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <array>

// Win Comaptibility
#if defined(_WIN32) || defined(_WIN64)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

using namespace physx;

#define DONT_USE_CUDA // Define this to disable CUDA support if you don't have an NVIDIA GPU or want to avoid GPU-specific code paths

// File paths
#define RESOUCES_PATH "resources/"
#define MODELS_PATH RESOUCES_PATH "models/"
#define SHADERS_PATH RESOUCES_PATH "shaders/"

#define MATERIALS_LIST_PATH "resources/materials.list"
#define MODELS_LIST_PATH "resources/models.list"


// Logging Macros
#define RF_LOG_ERROR(msg, ...) \
    TraceLog(LOG_ERROR, "[RayForce] [%s:%d]" msg, __FILE__, __LINE__, ##__VA_ARGS__)

#define RF_LOG_WARN(msg, ...) \
    TraceLog(LOG_WARNING, "[RayForce] [%s:%d] " msg, __FILE__, __LINE__, ##__VA_ARGS__)

#define RF_LOG_INFO(msg, ...) \
    TraceLog(LOG_INFO, "[RayForce] " msg, ##__VA_ARGS__)
