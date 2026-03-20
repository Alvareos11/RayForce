#pragma once
#include "globals.h"
#include "managers/model.h"

/**
 * Entity Class
 * The base class for all world objects. Handles the synchronization between
 * Raylib visual components and NVIDIA PhysX physical actors.
 */
class Entity {
protected:
    // Transformation Data
    PxTransform transform;

    Quaternion rotationQuat = { 0, 0, 0, 1 }; // Rotation Quat
    Vector3 position = { 0, 0, 0 };
    Vector3 rotation = { 0, 0, 0 }; // Euler angles (degrees)
    Vector3 scale = { 1, 1, 1 };
    Vector3 velocity = { 0, 0, 0 };

    // Visual Assets
    Model* model;                   // Pointer to the Raylib Model resource
    ModelID modelID;                // Unique identifier for the model asset

    // Physical Properties
    float mass = 10.0f;

public:
    /** * PhysX Dynamic Actor
     * Represents the physical body in the simulation.
     * Initialized via SetHitbox() for objects that require collision.
     */
    PxRigidBody* hitbox = nullptr;

    /**
     * Constructor
     * @param pos: Initial world position.
     * @param _modelID: The ID of the visual asset to be assigned.
     */
    Entity(Vector3 pos, ModelID _modelID);

    /**
     * Destructor
     * Ensures the associated PhysX actor is safely released from the simulation.
     */
    ~Entity();

    /**
     * PhysicsUpdate
     * Pulls the latest Transform/Pose from the PhysX simulation and
     * applies it to the internal position and rotation variables.
     * Call this only when needed to update position, rotation, and velocity data.
     */
    void PhysicsUpdate();

    /**
     * SetHitbox
     * Creates and attaches a physical body to the entity using the provided geometry.
     * @param geometry: The "cooked" physical shape (Box, Convex Mesh, etc).
     */
    void SetHitbox(PxGeometry* pgeometry, bool make_dynamic = true);

    /**
     * Sync
     * Pushes the current Entity transform (Visual) to the PhysX Actor (Physical).
     * Used for initial placement or manual teleports.
     */
    void Sync();

    /**
     * UpdateMatrixRender
     * Converts the current PhysX transform into a Raylib Matrix and submits it to the RenderManager.
     * This is what enables hardware instancing for efficient rendering of multiple entities using the same model.
     */
    void UpdateMatrixRender();

    /**
     * Render
     * Virtual methot for doing RayLib draw calls.
    */
    virtual void Render() {};

    /**
     * Update & Init
     * Virtual methods for specific entity logic (Input, AI, etc).
     */
    virtual void Update() {};
    virtual void Init() {};
};