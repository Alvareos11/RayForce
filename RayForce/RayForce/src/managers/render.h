#pragma once

#include "globals.h"

class RenderManager {
private:
    // Contains the Vbo size and ID
    struct Vbo {
        unsigned int VboId = 0;
        int VboSize = 0;
    };

    /**
     * The Render Buffer
     * Maps a Model pointer to a list of transformation matrices.
     * Each matrix represents the position, rotation, and scale of one instance.
     */
    std::unordered_map<Model*, std::vector<PxTransform>> renderBuffer;

    /**
     * The Instances VBO ID buffer
     * Stores the direcicon of the buffer
     */
    std::unordered_map<Mesh*, Vbo> instancesVboBuffer;
    
    // Prepare instance data buffer (instanceTransform) for instancing draw calls
    const int objectStride = 7 * sizeof(float);
public:
    RenderManager();
    ~RenderManager();

    // Adds a transforma to the queue for a specific model
    void AddModelToRenderBuffer(Model* model, PxTransform& t);

    // Clears all matrices from the buffer (usually called every frame)
    void ClearRenderBuffer();

    // Executes the GPU draw calls for all buffered models
    void RenderBuffer();

    // Render the intance mesh;
    void DrawMeshInstanced(Mesh* mesh, Material material, const PxTransform* transforms, int instances);
};