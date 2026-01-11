#include "scene.h"

/**
 * Scene Constructor
 * Initializes the physical world and sets up the environment.
 */
Scene::Scene() {
    // 1. Create the unique PhysX scene for this level
    physicsScene = Window::physicsManager->CreateScene();

    // 2. Setup Default Environment
    PxMaterial* defaultMaterial = Window::physicsManager->GetMaterial(MaterialID::Basic);

    // Creates an infinite physical plane at Y=0 (UP normal)
    PxRigidStatic* groundPlane = PxCreatePlane(*Window::physicsManager->Physics, PxPlane(0, 1, 0, 0), *defaultMaterial);

    if (!groundPlane) {
        std::cerr << "CRITICAL: Failed to create ground plane." << std::endl;
        return;
    }

    physicsScene->addActor(*groundPlane);
}

/**
 * Scene Destructor
 * Cleans up all entities and releases the PhysX scene handle.
 */
Scene::~Scene() {
    // Cleanup high-level entities
    for (Entity* entity : entities) {
        delete entity;
    }
    entities.clear();

    // Cleanup physics resources
    if (physicsScene) {
        physicsScene->release();
    }
}

/**
 * AddEntity
 * Registers an entity and attaches its physical actor to the simulation.
 */
void Scene::AddEntity(Entity* entity) {
    if (!entity) return;

    entities.push_back(entity);
    entity->Init();

    // Attach Physics Actor: Only if the pose is valid to avoid simulation crashes
    if (entity->hitbox != nullptr) {
        if (entity->hitbox->getGlobalPose().isValid()) {
            physicsScene->addActor(*entity->hitbox);
        }
    }

    entity->Sync(); // Immediate transform synchronization
}

/**
 * RemoveEntity
 * Unregisters an entity from the scene's update/render list.
 */
void Scene::RemoveEntity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}

// Fixed timestep settings for physics stability
float fixedStep = 1.0f / 60.0f;
float accumulator = 0.0f;

/**
 * Scene::Render
 * Advances simulation and processes the drawing queue.
 */
void Scene::Render() {
    float frameTime = GetFrameTime();

    // Stability Guard: Handle frame spikes or window freezing
    if (Window::isPaused || frameTime > 0.25 || frameTime < 0) {
        frameTime = fixedStep;
    }

    // Begin physics simulation step
    physicsScene->simulate(frameTime);

    DrawGrid(1000, 1.0f); // Spatial orientation helper

    // Queue entities for rendering
    for (Entity* entity : entities) {
        entity->Render();
    }

    // Final GPU draw call for all instanced models
    Window::renderManager->RenderBuffer();
}

/**
 * Scene::Update
 * Handles logic, input, physics synchronization, and entity lifecycle.
 */
void Scene::Update() {
    // 1. Input: Fullscreen Toggle Management
    if (IsKeyPressed(KEY_F11)) {
        if (!IsWindowFullscreen()) {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        }
        else {
            ToggleFullscreen();
            if (window != nullptr) SetWindowSize(window->width, window->height);
        }
    }

    // 2. Physics Sync: Wait for simulation results to be ready
    physicsScene->fetchResults(true);

    // 3. Scripting: Call custom tick logic for the scene
    Tick();

    // 4. Entity Lifecycle & Sync
    for (Entity* entity : entities) {
        if (entity == nullptr) {
            RemoveEntity(entity);
            continue;
        }

        // Pull new transform data from PhysX to Raylib
        entity->PhysicsUpdate();

        // Update entity-specific logic
        entity->Update();
    }

    // 5. Scene Switching: Load new scene if a transition was requested
    if (nextScene != nullptr) {
        Window::sceneManager->LoadScene(nextScene);
    }
}

/**
 * GetPhysicsScene
 * @return Raw pointer to the internal PhysX scene.
 */
PxScene* Scene::GetPhysicsScene() {
    return physicsScene;
}

/**
 * LoadScene
 * Sets the next scene to be loaded at the end of the current update frame.
 */
void Scene::LoadScene(Scene* scene) {
    nextScene = scene;
}