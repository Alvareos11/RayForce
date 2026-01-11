#include "test.h"
#include "../entities/testEntity.h"
#include "scene.h"

// Configuration variables for the stress test
ModelID model = ModelID::Cube;
int gridSize = 1;

/**
 * Test::Init
 * Sets up the initial state of the test environment.
 */
void Test::Init() {
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

bool toggleMouse = false;

/**
 * Test::Tick
 * Handles runtime input for debugging and scene management.
 */
void Test::Tick() {
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
}