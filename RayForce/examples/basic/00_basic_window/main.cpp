#include "RayForce.h"

int main() {
    // Window constructor: Initializes the Raylib context and sets hardware flags.
    window = new Window(800, 600, "Basic Window", FLAG_WINDOW_RESIZABLE);

    // Load a minimal default scene so the window has a valid physics world.
    window->sceneManager->LoadScene(new Scene());
    window->GameLoop();
    
    return 0;
}


