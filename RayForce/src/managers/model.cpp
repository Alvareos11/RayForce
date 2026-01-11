#include "model.h"
#include "physics.h"
#include "../window.h"

ModelManager::ModelManager() {
    // Loading shaders for instanced rendering (optimized for drawing many objects)
    shdr = LoadShader((SAHDERS_PATH + std::string("instancing.vs")).c_str(),
        (SAHDERS_PATH + std::string("basic.fs")).c_str());

    if (!IsShaderValid(shdr)) {
        TraceLog(LOG_ERROR, "Fatal Error! Shaders failed to load.");
        return;
    }

    // Assigning shader locations for the MVP matrix and instance transforms
    shdr.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shdr, "mvp");
    shdr.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shdr, "instanceTransform");
}

ModelManager::~ModelManager() {
    for (auto& pair : models) {
        UnloadModel(pair.first);
    }
    models.clear();
    UnloadShader(shdr);
}

Model& ModelManager::GetModel(ModelID id) {
    // Cache Check: return model if already loaded
    auto it = models.find(id);
    if (it != models.end()) {
        return it->second;
    }

    std::string path = std::string(MODELS_PATH + modelFilepaths.at(id));
    Model model;

    if (!FileExists(path.c_str())) {
        TraceLog(LOG_ERROR, "File not found: %s", path.c_str());
        goto error;
    }

    model = ::LoadModel(path.c_str());

    if (model.meshes == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load mesh data for: %s", path.c_str());
        goto error;
    }

    goto done;

error:
    // Fallback: load a purple cube so the developer knows the asset is missing
    model = ::LoadModelFromMesh(GenMeshCube(1, 1, 1));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = PURPLE;

done:
    // Apply the instancing shader and reset default color to white for all materials
    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        model.materials[i].shader = shdr;
    }

    models[id] = model;
    return models.at(id);
};

void ModelManager::UnloadModel(ModelID id) {
    auto it = models.find(id);
    if (it == models.end()) {
        TraceLog(LOG_WARNING, "Model ID %d not found in cache", id);
        return;
    }
    // Raylib's internal mesh/material cleanup would happen here if needed
    models.erase(it);
};

PxMaterial* ModelManager::GetModelMaterial(ModelID id) {
    // Bridges the visual model ID to a physical material (friction, restitution)
    MaterialID materialID = modelMaterials.at(id);
    return Window::physicsManager->GetMaterial(materialID);
}