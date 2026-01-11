#pragma once

#include "../globals.h"

class RenderManager {
private:
    /**
     * The Render Buffer
     * Maps a Model pointer to a list of transformation matrices.
     * Each matrix represents the position, rotation, and scale of one instance.
     */
    std::unordered_map<Model*, std::vector<Matrix>> renderBuffer;

public:
    RenderManager();
    ~RenderManager();

    // Adds a transformation matrix to the queue for a specific model
    void AddModelToRenderBuffer(Model* model, Matrix t);

    // Clears all matrices from the buffer (usually called every frame)
    void ClearRenderBuffer();

    // Executes the GPU draw calls for all buffered models
    void RenderBuffer();
};