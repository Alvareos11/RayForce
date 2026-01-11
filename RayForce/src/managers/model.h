#pragma once

#include "../globals.h"
#include "physics.h"

/**
 * ModelID Enum
 * Automatically populated from 'models.list'
 */
enum class ModelID : uint16_t {
#define MODEL_DEF(id, filepath, material) id,
#include "models.list"
#undef MODEL_DEF
};

class ModelManager {
private:
    // Storage for loaded Raylib Model structures
    std::unordered_map<ModelID, Model> models;

    // Mapping ModelIDs to their respective file paths on disk
    const std::map<ModelID, std::string> modelFilepaths = {
        #define MODEL_DEF(id, filepath, material) { ModelID::id, filepath },
        #include "models.list"
        #undef MODEL_DEF
    };

    // Mapping ModelIDs to their physical material properties
    const std::map<ModelID, MaterialID> modelMaterials = {
        #define MODEL_DEF(id, filepath, material) { ModelID::id, MaterialID::material },
        #include "models.list"
        #undef MODEL_DEF
    };

    Shader shdr; // Default shader applied to all managed models

public:
    ModelManager();
    ~ModelManager();

    /**
     * GetModel: Retrieves a model from the cache or loads it if not present.
     */
    Model& GetModel(ModelID id);

    /**
     * UnloadModel: Frees GPU/RAM resources for a specific model.
     */
    void UnloadModel(ModelID id);

    /**
     * GetModelMaterial: Returns the PhysX material associated with this model's ID.
     */
    PxMaterial* GetModelMaterial(ModelID id);
};