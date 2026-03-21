#pragma once

#include "globals.h"

enum class ShaderID : uint16_t {
    #define SHADER_DEF(id, _vs, _fs) id,
    #include "shaders.list"
    #undef SHADER_DEF
};

class ShaderManager {
public:
    // It contains the shaders paths
    struct ShaderPath {
        std::string vs;
        std::string fs;
    };
private:
    
    // Mapping ShaderIDs to their respective file paths on disk
    const std::map<ShaderID, ShaderPath> shadersPaths = {
        #define SHADER_DEF(id, _vs, _fs) { ShaderID::id, {_vs , _fs} },
        #include "shaders.list"
        #undef SHADER_DEF
    };

public:
    std::unordered_map<ShaderID, Shader> shaders;

    ShaderManager();
    ~ShaderManager();

    void AsingShader(const ShaderID &shader, Model& model);
};