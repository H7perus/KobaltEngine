#pragma once

#include "Engine/Entity/Entity.h"
#include "GpuInterface/Types/PipelineGraphics.h"
#include "GpuInterface/Types/Texture.h"

#include "GpuInterface/Types/ResourceSet.h"

#include "Mesh.h"

#include "Renderer/Types/EntityComponents/StaticMeshComponent.h"
#include "SDL3/SDL.h"

#include "glm/glm.hpp"
#include <memory>

namespace KE::R
{
class RenderContext
{

  public:
    RenderContext();
    VK::Buffer vertexPosBuffer;
    // will be interesting with compaction in the future;
    u32 currentVertexOffset = 0;

    std::map<std::string, std::shared_ptr<Mesh>> meshes_;

    std::shared_ptr<Mesh> errorModel;

    // does not account well for meshes that have a mix of rendering types (opaque, transparent, alpha test, etc...)
    // TODO: Find a solution for this. For now this is fine
    std::vector<std::pair<std::shared_ptr<KE::Entity>, std::shared_ptr<KE::R::StaticMeshComponent>>>
        staticOpaqueRenderables;


    std::vector<glm::mat3x4> transforms;

    VK::ResourceSet<VK::Buffer> transformBuffers;

    VK::ResourceSet<VK::Buffer> FrameDataBuffers;

    std::vector<std::shared_ptr<VK::PipelineGraphics>> pipelines;

    SDL_Window** window;

    // Normal per frame compute should be run on the graphics queue.
    vk::Queue graphics_queue_;
    vk::Queue compute_queue_;
    // TODO: implement this on a separate thread.
    vk::Queue transfer_queue_;

    VK::ResourceSet<VK::Texture> HDRFramebuffer;

    const u32 numFramesInFlight = 2;

    u32 currentFrameInFlight = 0;

    static RenderContext& GetInstance();

    std::shared_ptr<Mesh> AddMesh(RenderMesh loadedMesh, std::string meshName);

    u32 AddVerticesToPosBuffer(std::vector<glm::vec3> vertices);
};
} // namespace KE::R
