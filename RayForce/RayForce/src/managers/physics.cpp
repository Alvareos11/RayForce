#include "physics.h"
#include "model.h"
#include "window.h"

// Static members for PhysX SDK communication
PxDefaultAllocator PhysicsManager::Allocator;
PxDefaultErrorCallback PhysicsManager::ErrorCallback;

/**
 * PhysicsManager Constructor
 * Initializes the NVIDIA PhysX SDK, GPU acceleration, and simulation parameters.
 */
PhysicsManager::PhysicsManager() {
    // 1. Core SDK Initialization
    // Foundation provides the base for all PhysX allocations and error logging.
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);

    // Main Physics registry used to create materials, shapes, and actors.
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), true);

    // 2. Scene Description Configuration
    // Defines global simulation properties like gravity and optimization flags.
    SceneDesc = new PxSceneDesc(Physics->getTolerancesScale());
    SceneDesc->gravity = PxVec3(0.0f, -9.81f, 0.0f);
    SceneDesc->filterShader = PxDefaultSimulationFilterShader;

    // Enables 'Active Actor' tracking to only sync entities that moved during the last step.
    SceneDesc->flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

#if !defined(DONT_USE_CUDA)
    // 3. GPU Hardware Acceleration (CUDA)
    // Tries to offload the physics solver to an NVIDIA GPU if available.
    PxCudaContextManagerDesc cudaContextManagerDesc;
    CudaContextManager = PxCreateCudaContextManager(*Foundation, cudaContextManagerDesc, PxGetProfilerCallback());

    if (CudaContextManager && CudaContextManager->contextIsValid()) {
        SceneDesc->cudaContextManager = CudaContextManager;
        SceneDesc->flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
        SceneDesc->broadPhaseType = PxBroadPhaseType::eGPU;

        
        // Pool config
        SceneDesc->gpuDynamicsConfig.heapCapacity = 128 * 1024 * 1024; // 128MB
        
        // Temporal buffer
        SceneDesc->gpuDynamicsConfig.tempBufferCapacity = 64 * 1024 * 1024; // 64MB

        // Pach limit
        SceneDesc->gpuDynamicsConfig.maxRigidPatchCount = 1024 * 800; 
        
        // Max conatac capcity
        SceneDesc->gpuDynamicsConfig.maxRigidContactCount = 1024 * 1024 * 2;

        // Max colliosn extac
        SceneDesc->gpuDynamicsConfig.collisionStackSize = 128 * 1024 * 1024; // 128MB

        RF_LOG_INFO("GPU Accelertion eneable");
    }   else {
        RF_LOG_WARN("CUDA not detected. Falling back to CPU simulation.");
    }
#endif

    // 4. CPU Dispatcher (Multithreading)
    // Allocates worker threads for the simulation based on CPU core count.
    unsigned int optimalThreads = std::max(2u, std::thread::hardware_concurrency() / 2);
    Dispatcher = PxDefaultCpuDispatcherCreate(optimalThreads);
    SceneDesc->cpuDispatcher = Dispatcher;

    // 5. Cooking Module Setup
    // Configures mesh preprocessing (welding vertices and generating convex hulls).
    CookingParams = new PxCookingParams(Physics->getTolerancesScale());
    CookingParams->meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    CookingParams->meshWeldTolerance = 0.001f;
    CookingParams->meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
    CookingParams->midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
}

/**
 * PhysicsManager Destructor
 * Ensures a strict release order to prevent SDK memory corruption.
 */
PhysicsManager::~PhysicsManager() {
    if (SceneDesc) { delete SceneDesc; SceneDesc = nullptr; }

    // Clear and release all managed resources
    for (auto& pair : loadedMaterials) pair.second->release();
    loadedMaterials.clear();

    for (auto& pair : loadedGeometries) DeleteGeometry(pair.second);
    loadedGeometries.clear();

    for (auto& pair : loadedShapes) pair.second->release();
    loadedShapes.clear();

    if (CookingParams) { delete CookingParams ; CookingParams = nullptr; }
    if (Dispatcher) { Dispatcher->release() ; Dispatcher = nullptr; }

#if !defined(DONT_USE_CUDA)
    if (CudaContextManager) { CudaContextManager->release(); CudaContextManager = nullptr; }
#endif

    if (Physics) { Physics->release(); Physics = nullptr; }
    if (Foundation) { Foundation->release(); Foundation = nullptr; }
}

// --- Scene Management ---

PxScene* PhysicsManager::CreateScene() {
    return Physics->createScene(*SceneDesc);
}

PxScene* PhysicsManager::CreateScene(PxSceneDesc _SceneDesc) {
    if (SceneDesc) delete SceneDesc;
    SceneDesc = new PxSceneDesc(_SceneDesc);
    return CreateScene();
}

// --- Resource Management (Materials) ---

PxMaterial* PhysicsManager::GetMaterial(MaterialID id) {
    auto it = loadedMaterials.find(id);
    if (it != loadedMaterials.end()) return it->second;

    Vector3 props = materialsProperties.at(id);
    PxMaterial* material = Physics->createMaterial(props.x, props.y, props.z);

    loadedMaterials[id] = material;
    RF_LOG_INFO("Loaded material ID %d", static_cast<uint16_t>(id));
    return material;
}

void PhysicsManager::UnloadMaterial(MaterialID id) {
    auto it = loadedMaterials.find(id);
    if (it != loadedMaterials.end()) {
        it->second->release();
        loadedMaterials.erase(it);
    }
}

// --- Geometry & Mesh Cooking ---

PxGeometry* PhysicsManager::CreateGeometry(ModelID _modelID) {
    auto it = loadedGeometries.find(_modelID);
    if (it != loadedGeometries.end()) return it->second;

    Model* model = &Window::modelManager->GetModel(_modelID);
    if (!model || model->meshCount == 0) return nullptr;

    // Collect and unify vertex data from all sub-meshes for cooking
    std::vector<float> allVertices;
    for (int i = 0; i < model->meshCount; i++) {
        Mesh& m = model->meshes[i];
        for (int v = 0; v < m.vertexCount * 3; v++) allVertices.push_back(m.vertices[v]);
    }

    RF_LOG_INFO("Cooking Convex Mesh for Model ID %d", _modelID);

    // Setup Convex Hull descriptor
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = (PxU32)allVertices.size() / 3;
    convexDesc.points.stride = sizeof(float) * 3;
    convexDesc.points.data = allVertices.data();
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxDefaultMemoryOutputStream writeBuffer;
    PxConvexMeshCookingResult::Enum result;

    if (!PxCookConvexMesh(*CookingParams, convexDesc, writeBuffer, &result)) {
        RF_LOG_ERROR("Cooking failed with code: %d", (int)result);
        return nullptr;
    }

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxConvexMesh* convexMesh = Physics->createConvexMesh(readBuffer);

    PxGeometry* geometry = new PxConvexMeshGeometry(convexMesh);
    loadedGeometries[_modelID] = geometry;

    RF_LOG_INFO("Created Geometry asing to Model ID %d", (unsigned int)_modelID);

    return geometry;
}

void PhysicsManager::UnloadGeometry(ModelID _modelID) {
    auto it = loadedGeometries.find(_modelID);
    if (it != loadedGeometries.end()) {
        DeleteGeometry(it->second);
        loadedGeometries.erase(it);
    }
}

void PhysicsManager::DeleteGeometry(PxGeometry* geo) {
    if (!geo) {
        RF_LOG_WARN("Called with null Geometry");
        return; 
    }

    // Release internal cooked mesh data based on geometry type
    if (geo->getType() == PxGeometryType::eTRIANGLEMESH) {
        static_cast<PxTriangleMeshGeometry*>(geo)->triangleMesh->release();
    }
    else if (geo->getType() == PxGeometryType::eCONVEXMESH) {
        static_cast<PxConvexMeshGeometry*>(geo)->convexMesh->release();
    }

    delete geo; // Delete the PxGeometry heap-allocated wrapper
}

// --- Shapes & Collision Logic ---

PxShape* PhysicsManager::CreateShape(PxGeometry* geometry, PxMaterial* material) {
    if (!geometry || !material) { 
        RF_LOG_WARN("The geometry or the material pointers where null");
        return nullptr; 
    }

    auto it = loadedShapes.find(geometry);
    if (it != loadedShapes.end()) return it->second;

    // Creates the shape as exclusive (true) to bind it specifically to one actor
    PxShape* shape = Physics->createShape(*geometry, *material, true);
    loadedShapes[geometry] = shape;

    RF_LOG_INFO("Created Shape asing to Geometry ptr %d", (unsigned int)geometry);
    return shape;
}

void PhysicsManager::UnloadShape(PxGeometry* geometry) {
    auto it = loadedShapes.find(geometry);
    if (it != loadedShapes.end()) {
        it->second->release();
        loadedShapes.erase(it);
    }
}


/*
    * Converts a PhysX transform into a float array format suitable for GPU instancing.
    * The first 3 floats represent position (x, y, z), and the next 4 represent rotation as a quaternion (x, y, z, w).
    * This format is compact and efficient for shader use when rendering multiple instances of the same model.
    * @param transform: The PhysX transform to convert.
    * @parm out: float pointer to the output
*/
void PhysicsManager::TransformToFloatArray(const PxTransform& transform, float* out) {
    out[0] = transform.p.x; // Position X
    out[1] = transform.p.y; // Position Y
    out[2] = transform.p.z; // Position Z
    out[3] = transform.q.x; // Rotation Quaternion X
    out[4] = transform.q.y; // Rotation Quaternion Y
    out[5] = transform.q.z; // Rotation Quaternion Z
    out[6] = transform.q.w; // Rotation Quaternion W
}

// --- Main Engine Synchronization --- legacy usefull but slow

#include "../elements/entity.h"
void PhysicsManager::UpdateEntities(PxScene* scene) {
    // Only fetch actors that have undergone a transform change (performance optimization)
    PxU32 nbActiveActors;
    PxActor** activeActors = scene->getActiveActors(nbActiveActors);

    for (PxU32 i = 0; i < nbActiveActors; ++i) {
        // Retrieve the C++ Entity pointer stored in the actor's userData
        Entity* ent = static_cast<Entity*>(activeActors[i]->userData);
        if (ent) {
            ent->PhysicsUpdate(); // Syncs PhysX pose back to Raylib position/rotation
        }
    }
}
