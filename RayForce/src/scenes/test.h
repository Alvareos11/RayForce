#pragma once

#include "scene.h"

/**
 * Test Scene
 * A derived class used for prototyping mechanics and physics interactions.
 */
class Test : public Scene {
public:
    // Calls the parent constructor to initialize the PhysX world and ground plane
    Test() : Scene() {};

    // Triggered when the scene is first loaded to spawn entities
    void Init() override;

    // Triggered every frame to handle level-specific logic or win/loss conditions
    void Tick() override;
};