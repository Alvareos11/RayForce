#include "render.h"

RenderManager::RenderManager() {
    // Constructor currently empty, reserved for future GL state initialization
}

RenderManager::~RenderManager() {
    ClearRenderBuffer();
    renderBuffer.clear();
}

/**
 * AddModelToRenderBuffer
 * Queues a transformation matrix for a specific model.
 * @param model: Pointer to the Raylib Model asset.
 * @param t: The 4x4 Transformation Matrix (Position, Rotation, Scale).
 */
void RenderManager::AddModelToRenderBuffer(Model* model, Matrix t) {
    if (model == nullptr) {
        TraceLog(LOG_WARNING, "Model pointer was null in RenderManager::AddModelToRenderBuffer");
        return;
    }

    // emplace_back avoids extra copies by constructing the matrix directly in the vector
    renderBuffer[model].emplace_back(t);
}

/**
 * ClearRenderBuffer
 * Resets the matrix lists for all models to prepare for the next frame.
 */
void RenderManager::ClearRenderBuffer() {
    for (auto& pair : renderBuffer) {
        pair.second.clear();
    }
}

/**
 * RenderBuffer
 * The core drawing function. Iterates through each unique model and
 * sends all its stored instances to the GPU in a single batch.
 */
void RenderManager::RenderBuffer() {
    for (auto& pair : renderBuffer) {
        Model* model = pair.first;
        std::vector<Matrix>& matrices = pair.second;

        if (matrices.empty()) continue;

        int instanceCount = (int)matrices.size();
        Matrix* dataPtr = matrices.data(); // Pointer to the raw array of matrices

        // A model can have multiple meshes (sub-objects). Each must be drawn instanced.
        for (int i = 0; i < model->meshCount; i++) {
            int materialIndex = model->meshMaterial[i];

            // Raylib calls the internal OpenGL/Vulkan instancing commands here
            DrawMeshInstanced(
                model->meshes[i],
                model->materials[materialIndex],
                dataPtr,
                instanceCount
            );
        }

        // Clean up the buffer after rendering to avoid re-drawing the same positions next frame
        matrices.clear();
    }
}