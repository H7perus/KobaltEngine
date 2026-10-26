#include "RenderContext.h"
#include "GpuInterface/Types/ResourceSet.h"

#include "Renderer/Types/FrameData.h"
#include "vulkan/vulkan.hpp"

KE::R::RenderContext::RenderContext()
{
    vertexPosBuffer = VK::Buffer(0, 1000000, vk::BufferUsageFlagBits::eVertexBuffer,
                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    HDRFramebuffer = VK::ResourceSet<VK::Texture>({}, 0, 800, 600, vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,vk::MemoryPropertyFlagBits::eDeviceLocal, true);

    transformBuffers = VK::ResourceSet<VK::Buffer>({VK::FRAMEALIGNED, numFramesInFlight}, 0, sizeof(glm::mat3x4) * 100, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, true);
    
    FrameDataBuffers = VK::ResourceSet<VK::Buffer>({VK::FRAMEALIGNED, numFramesInFlight}, 0, sizeof(FrameData_t), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, true);
}


KE::R::RenderContext &KE::R::RenderContext::GetInstance()
{
    static RenderContext instance;
    return instance;
}
std::shared_ptr<KE::R::Mesh> KE::R::RenderContext::AddMesh(RenderMesh loadedMesh, std::string meshName)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    mesh->submeshes_.resize(loadedMesh.primitives.size());

    int primIndex = -1;
    for (auto& primitive : loadedMesh.primitives)
    {
        primIndex++;

        auto& submesh = mesh->submeshes_[primIndex];
        submesh.attributeDescs_.resize(1);
        submesh.attributeDescs_[0].binding = 0;
        submesh.attributeDescs_[0].format  = vk::Format::eR32G32B32Sfloat;
        submesh.attributeDescs_[0].offset  = AddVerticesToPosBuffer(primitive.pos);

        submesh.indices_ =
            VK::Buffer(0, 4 * primitive.indices.size(), vk::BufferUsageFlagBits::eIndexBuffer,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        u32* indexBufferPointer = (u32*)submesh.indices_.map();
        memcpy(indexBufferPointer, primitive.indices.data(), primitive.indices.size() * 4);
        submesh.indices_.unmap();

        submesh.count_ = primitive.indices.size();
    }

    meshes_.try_emplace(meshName, mesh);

    return mesh;
};

u32 KE::R::RenderContext::AddVerticesToPosBuffer(std::vector<glm::vec3> vertices)
{
    glm::vec3* vertexBuffPointer = (glm::vec3*)vertexPosBuffer.map() + currentVertexOffset;

    memcpy(vertexBuffPointer, vertices.data(), vertices.size() * 3 * 4);

    u32 returnOffset = currentVertexOffset;
    currentVertexOffset += vertices.size();
    return returnOffset;
};