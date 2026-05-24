#include "RayForce.h"

/**
 * TestEntity Class
 * A concrete implementation of the Entity base class used for
 * stress testing and verifying physics-visual synchronization.
 */
class TestEntity : public Entity {
public:
    /**
     * Constructor
     * Forwards position and model data to the base Entity constructor.
     * @param _pos: Initial spawn position in world space.
     * @param _model: The ID of the model asset to be rendered.
     */
    TestEntity(Vector3 _pos, ModelID _model) : Entity(_pos, _model) {}

    /**
     * Init
     * Called once the entity is added to the scene.
     */
    void Init() override {
        // 1. Create a physx geometry
        PxGeometry* geometry = Window::physicsManager->CreateGeometry(modelID);

        // 2. Initialize Hitbox
        if (geometry) {
            SetHitbox(geometry);
        }
    }
};

/**
 * Test Scene
 * A derived class used for prototyping mechanics and physics interactions.
 */
class Test : public Scene {
    bool toggleMouse = false;
    ModelID model = ModelID::Cube;
    int gridSize = 1;
public:
    // Calls the parent constructor to initialize the PhysX world and ground plane
    Test() : Scene() {};

    // Trigered affter the scen is loaded
    void Init() override {
        DisableCursor(); // Locks mouse for 3D camera control

        RF_LOG_INFO("Initializing Test Scene");
        RF_LOG_INFO("Creating %d entities", gridSize * gridSize * gridSize);

        // Triple nested loop to spawn a 3D grid of physical objects
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                for (int k = 0; k < gridSize; k++) {
                    // Calculates offset positions so the grid is centered and elevated
                    Vector3 pos = {
                        ((float)i - gridSize / 2.0f) * 3.0f,
                        (float)j * 3.0f + 10.0f,
                        ((float)k - gridSize / 2.0f) * 3.0f
                    };

                    Window::sceneManager->GetCurrentScene()->AddEntity(new TestEntity(pos, model));
                }
            }
        }

        RF_LOG_INFO("Finished creating entities.");
    }

    /**
     * Test::Tick
     * Handles runtime input for debugging and scene management.
     */
    void Tick() {
        // Model Swapping: Toggle between primitive and complex meshes
        if (IsKeyPressed(KEY_M)) {
            model = (model == ModelID::Cube) ? ModelID::DamagedHelmet : ModelID::Cube;
        }

        // Hot Reloading: Restarts the scene with more or fewer entities
        if (IsKeyPressed(KEY_R)) {
            LoadScene(new Test());
            RF_LOG_INFO("Reloading Test Scene");

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                gridSize -= (gridSize - 1 == 0) ? 0 : 1; // Minimum size of 1
            }
            else {
                gridSize++; // Increase complexity for next load
            }
        }

        // Cursor Management: Toggle between game control and UI/Window interaction
        if (IsKeyPressed(KEY_TAB)) {
            if (toggleMouse) {
                DisableCursor();
                toggleMouse = false;
            }
            else {
                EnableCursor();
                toggleMouse = true;
            }
        }

        // Toggles full screen
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
    }
};

/**
 * Scene Initialization
 * Post-initialization hook. Used to inject the first scene into the manager
 * once the OpenGL context is ready.
 */

int main() {
    // Window constructor: Initializes the Raylib context and sets hardware flags.
    window = new Window(800, 600, "Stress Test", FLAG_WINDOW_RESIZABLE);
    
    // Load initial scene
    window->sceneManager->LoadScene(new Test());
    
    window->GameLoop();
    
    return 0;
}

