#pragma once

#include "../globals.h"
#include "../entities/entity.h"
#include "../window.h"

class Scene {
protected:
    // PhysX specific members for the simulation environment
    PxScene* physicsScene = nullptr;
    PxRigidStatic* groundPlane = nullptr;

    // Collection of all active objects in the scene
    std::vector<Entity*> entities;
    Scene* nextScene = nullptr;

public:
    // Lifecycle hooks for derived classes
    virtual void Init() {};
    virtual void Tick() {}; // Custom logic per frame

    Scene();
    virtual ~Scene();

    void Unload();

    // Returns the PhysX scene pointer for external simulation control
    PxScene* GetPhysicsScene();

    void Render();
    void Update();

    // Entity lifecycle management
    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);

    // Scene transition management
    void LoadScene(Scene* scene);
};