#pragma once
#include "../globals.h"
#include "../managers/model.h"

/**
 * Entity Class
 * The base class for all world objects. Handles the synchronization between
 * Raylib visual components and NVIDIA PhysX physical actors.
 */
class Entity {
protected:
    // Transformation Data
    Matrix worldMatrix;             // Final transformation matrix for rendering
    Quaternion rotationQuat = { 0.0f, 0.0f, 0.0f, 1.0f }; // Internal orientation

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
    PxRigidDynamic* hitbox = nullptr;

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
     */
    void PhysicsUpdate();

    /**
     * SetHitbox
     * Creates and attaches a physical body to the entity using the provided geometry.
     * @param geometry: The "cooked" physical shape (Box, Convex Mesh, etc).
     */
    void SetHitbox(PxGeometry* geometry);

    /**
     * Sync
     * Pushes the current Entity transform (Visual) to the PhysX Actor (Physical).
     * Used for initial placement or manual teleports.
     */
    void Sync();

    /**
     * Render
     * Submits the entity's current world matrix to the RenderManager's instancing buffer.
     */
    void Render();

    /**
     * Update & Init
     * Virtual methods for specific entity logic (Input, AI, etc).
     */
    virtual void Update();
    virtual void Init() {};
};