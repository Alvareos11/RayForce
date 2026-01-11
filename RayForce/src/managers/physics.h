#pragma once

#include "../globals.h"

/**
 * MaterialID Enum
 * Generated from 'materials.list' to define physical properties.
 */
enum class MaterialID : uint16_t {
#define MATERIAL_DEF(id, ...) id,
#include "materials.list"
#undef MATERIAL_DEF
};

class PhysicsManager {
private:
    // Raw properties (Friction, Static Friction, Restitution) from the list
    const std::map<MaterialID, Vector3> materialsProperties = {
        #define MATERIAL_DEF(id, ...) { MaterialID::id, {__VA_ARGS__} },
        #include "materials.list"
        #undef MATERIAL_DEF
    };

    // Cache to prevent redundant PhysX object creation
    std::unordered_map<MaterialID, PxMaterial*> loadedMaterials;
    std::unordered_map<uint16_t, PxGeometry*> loadedGeometries;
    std::unordered_map<PxGeometry*, PxShape*> loadedShapes;

public:
    static PxDefaultAllocator Allocator;
    static PxDefaultErrorCallback ErrorCallback;

#if !defined(DONT_USE_CUDA)
    physx::PxCudaContextManager* CudaContextManager = nullptr; // Hardware acceleration
#endif

    PxSceneDesc* SceneDesc = nullptr;
    PxFoundation* Foundation = nullptr; // SDK Root
    PxPhysics* Physics = nullptr;       // Simulation Registry
    PxDefaultCpuDispatcher* Dispatcher = nullptr; // Multithreading controller
    PxCooking* Cooking = nullptr;       // Mesh-to-Physics collider generator

    PhysicsManager();
    ~PhysicsManager();

    // Scene creation logic
    PxScene* CreateScene();
    PxScene* CreateScene(PxSceneDesc _SceneDesc);

    // Resource Management
    PxMaterial* GetMaterial(MaterialID id);
    void UnloadMaterial(MaterialID id);

    // Collision geometry (Box, Sphere, Convex Mesh)
    PxGeometry* CreateGeometry(uint16_t _modelID);
    void UnloadGeometry(uint16_t _modelID);
    void DeleteGeometry(PxGeometry* geo);

    // Shapes combine Geometry + Material for Actors
    PxShape* CreateShape(PxGeometry* geometry, PxMaterial* material);
    void UnloadShape(PxGeometry* geometry);

    // Syncs PhysX Actor transforms back to Entity objects
    void UpdateEntities(PxScene* scene);
};