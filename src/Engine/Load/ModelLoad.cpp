#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ModelLoad.h"

std::optional<RenderMesh> LoadGLTFAsRenderMesh(std::string path)
{
    using namespace tinygltf;
    RenderMesh result;

    Model model;


    TinyGLTF    loader;
    std::string error;
    std::string warn;

    bool success = loader.LoadASCIIFromFile(&model, &error, &warn, path);
    if(!success)
    {
        return std::optional<RenderMesh>();
    }
    // assumed for now
    const tinygltf::Mesh& mesh = model.meshes[0];

    result.primitives.resize(mesh.primitives.size());
    i32 primitive_index = -1;


    for (const auto& primitive : mesh.primitives)
    {
        primitive_index++;
        auto& renderPrimitive = result.primitives[primitive_index];

        if (primitive.attributes.count("POSITION"))
        {
            int                         accessorIdx = primitive.attributes.at("POSITION");
            const tinygltf::Accessor&   accessor    = model.accessors[accessorIdx];
            const tinygltf::BufferView& bufferView  = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer&     buffer      = model.buffers[bufferView.buffer];

            // Pointer to the start of position data
            const float* positions =
                reinterpret_cast<const float*>(buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

            // renderPrimitive.pos.resize(accessor.count);
            for (size_t i = 0; i < accessor.count; ++i)
            {
                // POSITION is always VEC3 float in glTF
                renderPrimitive.pos.push_back(
                    glm::vec3(-positions[i * 3 + 0], positions[i * 3 + 2], positions[i * 3 + 1]));
            }
        }

        if (primitive.indices >= 0)
        {
            const tinygltf::Accessor&   accessor   = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer&     buffer     = model.buffers[bufferView.buffer];

            const uint8_t* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

            for (size_t i = 0; i < accessor.count; ++i)
            {
                uint32_t index = 0;
                switch (accessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    index = data[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    index = reinterpret_cast<const uint16_t*>(data)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    index = reinterpret_cast<const uint32_t*>(data)[i];
                    break;
                }
                renderPrimitive.indices.push_back(index);
            }
        }
    }

    return std::optional<RenderMesh>(result);
}

// std::optional<RenderMesh> LoadGLTFAsRenderMeshWithMaterial(std::string path);
