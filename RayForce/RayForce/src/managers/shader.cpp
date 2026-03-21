#include "shader.h"

// Loading all shaders
ShaderManager::ShaderManager(){
    RF_LOG_INFO("Loading Shaders");
    for (auto const& [id, path] : shadersPaths){
        // Load the shaders
        Shader shdr = LoadShader((SHADERS_PATH + path.vs).c_str(), (SHADERS_PATH + path.fs).c_str());

        // Check if the shader is drunk
        if (!IsShaderReady(shdr)){
            RF_LOG_WARN("Failed to load shaders, %s, %s", path.vs.c_str(), path.fs.c_str());
            continue;
        }

        // Assigning shader locations for the MVP matrix
        shdr.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shdr, "mvp");

        // Save the shader
        shaders[id] = shdr;

        RF_LOG_INFO("Loaded Shader %d", id);
    }
}

ShaderManager::~ShaderManager(){
    for (auto& shader : shaders ){
        UnloadShader(shader.second);
    }
}

void ShaderManager::AsingShader(const ShaderID& shader, Model& model) {
    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        model.materials[i].shader = shaders[shader];
    }
}