#include "testEntity.h"
#include "../managers/physics.h"
#include "../window.h"

/**
 * Init
 * Specifically handles the setup for the TestEntity instance.
 * It retrieves cooked geometry from the PhysicsManager based on the ModelID.
 */
void TestEntity::Init() {
    // 1. Retrieve Physics Geometry
    // We cast the ModelID to a uint16_t to fetch the pre-cooked 
    // convex hull or triangle mesh from the manager's cache.
    PxGeometry* geometry = Window::physicsManager->CreateGeometry((uint16_t)modelID);

    // 2. Initialize Hitbox
    // Assigns the geometry to the PhysX PxRigidDynamic actor.
    // This handles mass calculation, inertia, and collision offsets.
    if (geometry != nullptr) {
        SetHitbox(geometry);
    }

    /* // Optional: Randomized Spawning Logic
    // Useful for stress testing the engine with thousands of entities.
    position = { (float)GetRandomValue(-20, 20), 5.0f + GetRandomValue(0, 20), (float)GetRandomValue(-20, 20)};
    velocity = { (float)GetRandomValue(-10, 10), (float)GetRandomValue(-10, 10), (float)GetRandomValue(-10, 10)};
    Sync(); // Push randomized position to the physical actor
    */
}

/**
 * Update
 * Handles frame-by-frame logic specific to this entity type.
 */
void TestEntity::Update() {
    // Future: Add logic for AI, interactions, or state changes here.
}