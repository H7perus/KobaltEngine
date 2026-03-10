#pragma once

#include "../../VkGpuInterface.h"
#include "../SwapchainOutput.h"
#include "../TaskNode.h"


#include "vulkan/vulkan.hpp"

#include "RendererDLL.h"

namespace KE::VK
{
// I believe this was a mistake. I will keep frame end and present as part of the swapchain outside of the graph for
// now.
class GPUI_DLL_API RenderTriangle : public ITaskNode
{
    u32 swapchainIndex_;
    //ugly, for testing.
    VkGpuInterface* gpuInterface_;
  public:
    RenderTriangle(u32 swapchainIndex, VkGpuInterface* gpuInterface);

    ~RenderTriangle() = default;

    virtual void Execute(vk::CommandBuffer cmd) override;
};
} // namespace KE::VK