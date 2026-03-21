#include "RayForce.h"
#include "test.h"

int main() {
    // Window constructor: Initializes the Raylib context and sets hardware flags.
    window = new Window(800, 600, "Brick Battle", FLAG_WINDOW_RESIZABLE);
    window->GameLoop();
}

/**
 * Window::Init
 * Post-initialization hook. Used to inject the first scene into the manager
 * once the OpenGL context is ready.
 */
void Window::Init() {
    // Scene redirection: Decouples window creation from game logic.
    sceneManager->LoadScene(new Test());
}