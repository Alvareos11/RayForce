#include "window.h"
#include "scenes/scene.h"

// Static member initialization
SceneManager* Window::sceneManager = nullptr;
ModelManager* Window::modelManager = nullptr;
PhysicsManager* Window::physicsManager = nullptr;
RenderManager* Window::renderManager = nullptr;
bool Window::isPaused = false;
Window* window = nullptr;

#if defined(_WIN32) || defined(_WIN64)
// External reference to the Windows Hook function
extern "C" void AttachHook(void* windowHandle, bool* pauseFlag);
#endif

Window::Window(int _width, int _height, const std::string& title, unsigned int flags) {
    width = _width;
    height = _height;

    // 1. Setup Graphics Context
    InitWindow(width, height, title.c_str());
    while (!IsWindowReady()); // Ensure GL context is active
    SetConfigFlags(flags);
    SetTargetFPS(60);

#if defined(_WIN32) || defined(_WIN64)
    // 2. Attach the OS Hook to handle pausing during window drag/resize
    AttachHook(GetWindowHandle(), &isPaused);
#endif

    // 3. Camera Setup (Perspective View)
    camera = { 0 };
    camera.position = { 10.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 4. Initialize Managers
    sceneManager = new SceneManager();
    modelManager = new ModelManager();
    physicsManager = new PhysicsManager();
    renderManager = new RenderManager();

    Init(); // Load initial scene

    // Initial physics step to prime the simulation
    sceneManager->GetCurrentScene()->GetPhysicsScene()->simulate(1.f / 60.f);

    // 5. Main Game Loop
    while (!WindowShouldClose()) {
        if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // Core Update Logic
        sceneManager->Update();

        // Core Rendering Logic
        BeginDrawing();
            ClearBackground(BLACK);
            
            BeginMode3D(camera);
            
            sceneManager->Render();
            
            EndMode3D();
            
            DrawFPS(10, 10);
        EndDrawing();
    }
}

Window::~Window() {
    // Ordered cleanup of manager memory
    if (renderManager) delete renderManager;
    if (physicsManager) delete physicsManager;
    if (sceneManager) delete sceneManager;
    if (modelManager) delete modelManager;

    CloseWindow(); // Close Raylib context
}