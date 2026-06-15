#pragma once

#include "GpuInterface/Types/PipelineGraphics.h"
#include "Mesh.h"

#include "SDL3/SDL.h"

#include "glm/glm.hpp"

namespace KE::R
{
class RenderContext
{

  public:
    RenderContext();
    VK::Buffer vertexPosBuffer;


    std::map<std::string, std::shared_ptr<Mesh>> meshes_;

    std::vector<glm::mat3x4> transforms_;


    std::vector<std::shared_ptr<VK::PipelineGraphics>> pipelines_;

    SDL_Window** window;

    //Normal per frame compute should be run on the graphics queue.
    vk::Queue graphics_queue_;
    vk::Queue compute_queue_;
    //TODO: implement this on a separate thread.
    vk::Queue transfer_queue_;


    static RenderContext& GetInstance();
};
} // namespace KE::R
