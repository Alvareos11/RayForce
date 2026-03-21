#pragma once

#include "globals.h"
#include "physics.h"
#include "shader.h"

/**
 * ModelID Enum
 * Automatically populated from 'models.list'
 */
enum class ModelID : uint16_t {
#define MODEL_DEF(id, filepath, material, shader) id,
#include "models.list"
#undef MODEL_DEF
};

class ModelManager {
private:
    // Storage for loaded Raylib Model structures
    std::unordered_map<ModelID, Model> models;

    // Mapping ModelIDs to their respective file paths on disk
    const std::map<ModelID, std::string> modelFilepaths = {
        #define MODEL_DEF(id, filepath, material, shader) { ModelID::id, filepath },
        #include "models.list"
        #undef MODEL_DEF
    };

    // Mapping ModelIDs to their physical material properties
    const std::map<ModelID, MaterialID> modelMaterials = {
        #define MODEL_DEF(id, filepath, material, shader) { ModelID::id, MaterialID::material },
        #include "models.list"
        #undef MODEL_DEF
    };

    // Mapping ModelsIDs to ther shader 
    const std::map<ModelID, ShaderID> modelShaders = {
        #define MODEL_DEF(id, filepath, material, _shader) { ModelID::id, ShaderID::_shader },
        #include "models.list"
        #undef MODEL_DEF
    };

public:
    ModelManager();
    ~ModelManager();

    /**
     * GetModel: Retrieves a model from the cache or loads it if not present.
     * @param id: Model ID
     * @return: Model
     */
    Model& GetModel(ModelID id);

    /**
     * UnloadModel: Frees GPU/RAM resources for a specific model.
     * @param id: Model ID
     */
    void UnloadModel(ModelID id);

    /**
     * GetModelMaterial: Returns the PhysX material associated with this model's ID.
     * @param id: Model ID
     * @return: Physx Material
     */
    PxMaterial* GetModelMaterial(ModelID id);
};