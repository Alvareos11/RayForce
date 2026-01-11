#pragma once

#include "../globals.h"

class Scene;

/**
 * SceneManager
 * Controls the high-level state of the application.
 * Orchestrates updates and rendering for the currently active level.
 */
class SceneManager {
private:
    Scene* currentScene = nullptr;

public:
    ~SceneManager();

    /**
     * LoadScene: Replaces the current scene with a new one.
     * @param scene: Pointer to the new Scene instance to be managed.
     */
    void LoadScene(Scene* scene);

    /**
     * GetCurrentScene: Provides access to the active scene's data and physics world.
     */
    Scene* GetCurrentScene();

    /**
     * UnloadScene: Safely deallocates the current scene's memory.
     */
    void UnloadScene();

    // Propagation methods: These pass the main loop calls down to the active scene
    void Render();
    void Update();
};