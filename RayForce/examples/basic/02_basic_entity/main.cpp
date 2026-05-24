#include "RayForce.h"

class MyEntity : public Entity {
public:
    MyEntity(Vector3 _pos) : Entity(_pos, ModelID::BOX) {}
};

class MyScene : public Scene {
    public:
    MyScene() : Scene() {};

    void Init() override {
        DisableCursor();

        AddEntity(new MyEntity({ 0, 5, 0 })); // Spawn a box entity above the ground
    }
};

int main() {
    // Window constructor: Initializes the Raylib context and sets hardware flags.
    window = new Window(800, 600, "Basic Entity", FLAG_WINDOW_RESIZABLE);
    
    // Load initial scene
    window->sceneManager->LoadScene(new MyScene());
    
    window->GameLoop();
    
    return 0;
}