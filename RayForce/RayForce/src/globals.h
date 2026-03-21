#pragma once

#include <raylib.h>
#include <rlgl.h>
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

// Redudant thigs I have to add to make teh custom RayLib renderer because #include is dumb and a pain in the ass
#ifndef MAX_MATERIAL_MAPS
    #define MAX_MATERIAL_MAPS       12 
#endif

// Shader Stuff
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION    0
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD    1
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL      2
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR       3
#endif
    #ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT         4
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2   5
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES     6
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES 7
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS 8
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM 9
#endif

// Shader compatibility stuff
#ifndef SHADER_LOC_VERTEX_INSTANCETRANSFORM
    #define SHADER_LOC_VERTEX_INSTANCETRANSFORM 28
#endif