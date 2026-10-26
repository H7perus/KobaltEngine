#pragma once
#include <unordered_map>
#include <typeindex>

#include "GLTexture.h"
#include "tiny_gltf.h"
#include "glm/glm.hpp"
#include "GL/glew.h"

#include "../rendererdll.h"

typedef struct alignas(16) {
    glm::vec4 color_factor;
    glm::vec4 emissive_factor; //we don't have area light support but we can still make such materials look bright/glowing
    GLuint64 albedo;
    GLuint64 emission;

    GLuint64 normal;
    GLuint64 metallicity;
    //GLuint64 PADDING;
} PBRMat;


inline std::unordered_map<std::type_index, uint32_t> MaterialSizes{ {typeid(PBRMat), sizeof(PBRMat)} };

RENDERER_DLL_API int GetMatSize(std::type_index materialID);

RENDERER_DLL_API PBRMat GetMaterial(tinygltf::Model& model, int material_index);


//Placeholder for now
class Material
{

};

