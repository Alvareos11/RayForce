#pragma once
#include "entity.h"

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
     * Overrides the base Init to set up specific collision geometry
     * (e.g., cooking a convex mesh or assigning a box hitbox).
     */
    void Init() override;

    /**
     * Update
     * Overrides the base Update to handle frame-by-frame logic
     * such as input response or AI behavior.
     */
    void Update() override;
};