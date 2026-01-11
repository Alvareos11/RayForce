#include "entity.h"
#include "../window.h"
#include "../managers/model.h"
#include "../managers/physics.h"
#include "../managers/render.h"

/**
 * Entity Constructor
 * Links the entity to its visual resource via the ModelManager.
 */
Entity::Entity(Vector3 pos, ModelID _modelID) : position(pos), modelID(_modelID) {
    model = &Window::modelManager->GetModel(modelID);
}

/**
 * Entity Destructor
 * Ensures the physical actor is removed from the PhysX simulation to prevent leaks.
 */
Entity::~Entity() {
    if (hitbox) {
        hitbox->release();
        hitbox = nullptr;
    }
}

/**
 * PhysicsUpdate
 * Pulls the latest simulation results from PhysX and translates them into Raylib data.
 */
void Entity::PhysicsUpdate() {
    if (hitbox != nullptr) {
        // Retrieve the global transform from the PhysX actor
        PxTransform transform = hitbox->getGlobalPose();

        // 1. Sync Position and Orientation (Quaternion)
        position = { transform.p.x, transform.p.y, transform.p.z };
        rotationQuat = { transform.q.x, transform.q.y, transform.q.z, transform.q.w };

        // 2. Convert Quaternion to Euler angles (Degrees) for high-level logic
        Vector3 eulerRadians = QuaternionToEuler(rotationQuat);
        rotation.x = eulerRadians.x * RAD2DEG;
        rotation.y = eulerRadians.y * RAD2DEG;
        rotation.z = eulerRadians.z * RAD2DEG;

        // 3. Convert PhysX 4x4 Matrix to Raylib Matrix for efficient rendering
        PxMat44 physxMat(transform);

        // Map column-major PhysX matrix to Raylib's worldMatrix structure
        // Rotation and Scaling (3x3 Block)
        worldMatrix.m0 = physxMat.column0.x;
        worldMatrix.m1 = physxMat.column0.y;
        worldMatrix.m2 = physxMat.column0.z;
        worldMatrix.m3 = physxMat.column0.w;

        worldMatrix.m4 = physxMat.column1.x;
        worldMatrix.m5 = physxMat.column1.y;
        worldMatrix.m6 = physxMat.column1.z;
        worldMatrix.m7 = physxMat.column1.w;

        worldMatrix.m8 = physxMat.column2.x;
        worldMatrix.m9 = physxMat.column2.y;
        worldMatrix.m10 = physxMat.column2.z;
        worldMatrix.m11 = physxMat.column2.w;

        // Translation/Position
        worldMatrix.m12 = physxMat.column3.x;
        worldMatrix.m13 = physxMat.column3.y;
        worldMatrix.m14 = physxMat.column3.z;
        worldMatrix.m15 = 1.0f; // Homogeneous coordinates

        // Sync Velocity for AI or game logic usage
        PxVec3 v = hitbox->getLinearVelocity();
        velocity = { v.x, v.y, v.z };
    }
}

/**
 * Sync
 * Manual synchronization from the Entity properties to the PhysX Actor.
 * Useful for teleporting or resetting object states.
 */
void Entity::Sync() {
    if (hitbox != nullptr) {
        PxQuat q = { rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w };
        hitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z), { q }));
        hitbox->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
    }
}

/**
 * Render
 * Submits the current world matrix to the RenderManager for Hardware Instancing.
 */
void Entity::Render() {
    if (model) {
        Window::renderManager->AddModelToRenderBuffer(model, worldMatrix);
    }
}

/**
 * SetHitbox
 * Creates a physical body (RigidDynamic) and attaches it to the entity.
 */
void Entity::SetHitbox(PxGeometry* pgeometry) {
    if (pgeometry == nullptr) {
        RF_LOG_WARN("Called with null geometry");
        return;
    }

    PxGeometry& geometry = *pgeometry;

    // 1. Validate initial state
    if (!std::isfinite(position.x)) position = { 0, 0, 0 };
    if (mass <= 0.0f) mass = 1.0f;

    // 2. Initialize Actor with current position and Identity rotation
    PxTransform initialTransform(PxVec3(position.x, position.y, position.z));
    initialTransform.q = PxQuat(PxIdentity);

    if (hitbox == nullptr) {
        hitbox = Window::physicsManager->Physics->createRigidDynamic(initialTransform);
        // Link the PhysX actor back to this instance for collision callbacks
        hitbox->userData = (void*)this;
    }

    // 3. Create Shape and Material
    PxMaterial* material = Window::modelManager->GetModelMaterial(modelID);
    PxShape* shape = PxRigidActorExt::createExclusiveShape(*hitbox, geometry, *material);

    // Fine-tune collision offsets for stability
    shape->setContactOffset(0.02f);
    shape->setRestOffset(0.0f);

    // 4. Finalize physical properties
    PxRigidBodyExt::setMassAndUpdateInertia(*hitbox, mass);
    hitbox->setLinearVelocity(PxVec3(0));
    hitbox->setAngularVelocity(PxVec3(0));

    // Performance: Object will stop calculating when movement is minimal
    hitbox->setSleepThreshold(0.2f);
}

void Entity::Update() {
    // Virtual method - intended for override in child classes
}