#include "RayForce.h"

class MyScene : public Scene {
    public:
    MyScene() : Scene() {};

    void Init() override {
        DisableCursor();
    }
};

int main() {
    // Window constructor: Initializes the Raylib context and sets hardware flags.
    window = new Window(800, 600, "Basic Scene", FLAG_WINDOW_RESIZABLE);
    
    // Load initial scene
    window->sceneManager->LoadScene(new MyScene());
    
    window->GameLoop();
    
    return 0;
}