#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "GpuInterface/Types/Buffer.h"
#include "GpuInterface/VkGpuInterface.h"
#include "GpuInterface/Types/SwapchainOutput.h"
#include "GpuInterface/Types/TaskNode.h"

#include "Renderer/Types/Submesh.h"


#include "vulkan/vulkan.hpp"

#include "GpuInterface/GpuInterface.h"
#include "Renderer/Renderer.h"

#include "RendererDLL.h"

namespace KE::VK
{
// I believe this was a mistake. I will keep frame end and present as part of the swapchain outside of the graph for
// now.
class RENDERER_DLL_API RenderTriangle : public ITaskNode
{
    u32 swapchainIndex_;
    //ugly, for testing.
    VkGpuInterface* gpuInterface_;

    public:
    KE::R::Submesh* mesh;

  public:
    RenderTriangle();

    RenderTriangle(u32 swapchainIndex, VkGpuInterface* gpuInterface);
    
    ~RenderTriangle() = default;

    virtual void Execute(vk::CommandBuffer cmd) override;
};

} // namespace KE::VK