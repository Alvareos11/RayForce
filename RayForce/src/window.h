#pragma once

#include "globals.h"

#include "managers/model.h"
#include "managers/scenes.h"
#include "managers/physics.h"
#include "managers/render.h"

class Window {
protected:
    // Internal lifecycle methods
    void Render();
    void Update();
    void Init();

public:
    int width, height;
    Camera camera; // Raylib Camera3D/2D structure for view matrix management

    Window(int _width, int _height, const std::string& title, unsigned int flags);
    ~Window();

    // Static Managers: Shared across all scenes and entities
    static SceneManager* sceneManager;
    static ModelManager* modelManager;
    static PhysicsManager* physicsManager;
    static RenderManager* renderManager;

    // Global state for handling OS-level interruptions (resizing/moving)
    static bool isPaused;
};

// Global pointer to the active window instance
extern Window* window;