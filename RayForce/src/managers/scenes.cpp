#include "scenes.h"
#include "../scenes/scene.h"

SceneManager::~SceneManager() {
    UnloadScene();
}

/**
 * LoadScene
 * Safely replaces the active scene with a new one.
 */
void SceneManager::LoadScene(Scene* scene) {
    if (scene == nullptr) return;

    // Clean up previous scene resources before switching
    if (currentScene != nullptr) {
        UnloadScene();
    }

    currentScene = scene;

    // Trigger the initial setup (spawning players, setting up world)
    currentScene->Init();
}

/**
 * UnloadScene
 * Deletes the current scene object, triggering its destructor
 * which cleans up PhysX actors and Entities.
 */
void SceneManager::UnloadScene() {
    if (currentScene == nullptr) return;

    delete currentScene;
    currentScene = nullptr;
}

void SceneManager::Render() {
    if (currentScene == nullptr) return;
    currentScene->Render();
}

void SceneManager::Update() {
    if (currentScene == nullptr) return;
    currentScene->Update();
}

Scene* SceneManager::GetCurrentScene() {
    return currentScene;
}