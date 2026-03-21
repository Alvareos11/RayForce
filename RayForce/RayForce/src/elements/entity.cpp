#include "entity.h"
#include "window.h"
#include "managers/model.h"
#include "managers/physics.h"
#include "managers/render.h"

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
        // Sync Position and Orientation (Quaternion)
        position = { transform.p.x, transform.p.y, transform.p.z };
        rotationQuat = { transform.q.x, transform.q.y, transform.q.z, transform.q.w };

        // Convert Quaternion to Euler angles (Degrees) for high-level logic
        Vector3 eulerRadians = QuaternionToEuler(rotationQuat);
        rotation.x = eulerRadians.x * RAD2DEG;
        rotation.y = eulerRadians.y * RAD2DEG;
        rotation.z = eulerRadians.z * RAD2DEG;

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
    PxRigidDynamic* dynamicHitbox = (PxRigidDynamic*)hitbox;
    if (hitbox != nullptr) {
        PxQuat q = { rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w };
        dynamicHitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z), { q }));
        dynamicHitbox->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
    }
}

/**
 * UpdateMatrixRender
 * Submits the current world matrix to the RenderManager for Hardware Instancing.
 */
void Entity::UpdateMatrixRender() {
    if (model && hitbox) {
        // Get the trasnform
        transform = hitbox->getGlobalPose();
        
        /*
        // Convert PhysX 4x4 Matrix to Raylib Matrix for efficient rendering
        PxMat44 physxMat(transform);
        Matrix renderMat;

        
        // Map column-major PhysX matrix to Raylib's worldMatrix structure
        // Rotation and Scaling (3x3 Block)
        renderMat.m0 = physxMat.column0.x;
        renderMat.m1 = physxMat.column0.y;
        renderMat.m2 = physxMat.column0.z;
        renderMat.m3 = physxMat.column0.w;

        renderMat.m4 = physxMat.column1.x;
        renderMat.m5 = physxMat.column1.y;
        renderMat.m6 = physxMat.column1.z;
        renderMat.m7 = physxMat.column1.w;

        renderMat.m8 = physxMat.column2.x;
        renderMat.m9 = physxMat.column2.y;
        renderMat.m10 = physxMat.column2.z;
        renderMat.m11 = physxMat.column2.w;

        // Translation/Position
        renderMat.m12 = physxMat.column3.x;
        renderMat.m13 = physxMat.column3.y;
        renderMat.m14 = physxMat.column3.z;
        renderMat.m15 = 1.0f; // Homogeneous coordinates
        
        
        Window::renderManager->AddModelToRenderBuffer(model, renderMat);
        */
       
        Window::renderManager->AddModelToRenderBuffer(model, transform);
    }
}

/**
 * SetHitbox
 * Creates a physical body and attaches it to the entity.
 */
void Entity::SetHitbox(PxGeometry* pgeometry, bool make_dynamic) {
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

        // Make the rigid body dynamic or static
        if (make_dynamic) {
            PxRigidDynamic* dynamicHitbox = Window::physicsManager->Physics->createRigidDynamic(initialTransform);
            
            // Movement properties
            dynamicHitbox->setLinearVelocity(PxVec3(0));
            dynamicHitbox->setAngularVelocity(PxVec3(0));

            PxRigidBodyExt::setMassAndUpdateInertia(*dynamicHitbox, mass);
            
            hitbox = dynamicHitbox;
        }
        else {
            hitbox = Window::physicsManager->Physics->createRigidDynamic(initialTransform);
        }
        // Link the PhysX actor back to this instance for collision callbacks
        hitbox->userData = (void*)this;
    }

    // 3. Create Shape and Material
    PxMaterial* material = Window::modelManager->GetModelMaterial(modelID);
    PxShape* shape = Window::physicsManager->CreateShape(&geometry, material);

    // Fine-tune collision offsets for stability
    shape->setContactOffset(0.02f);
    shape->setRestOffset(0.0f);

    // Asing the shape to the hitbox
    hitbox->attachShape(*shape);

    // Performance: Object will stop calculating when movement is minimal
    if (make_dynamic) ((PxRigidDynamic*)hitbox)->setSleepThreshold(0.2f);
}