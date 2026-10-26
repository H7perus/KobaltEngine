#pragma once

#include "GpuInterface/Types/TaskNode.h"

#include "GpuInterface/Types/PipelineGraphics.h"

#include "vulkan/vulkan.hpp"

#include "RendererDLL.h"

namespace KE::R
{
class RENDERER_DLL_API TonemapAndPresent : public VK::ITaskNode
{
  public:
    TonemapAndPresent();
    ~TonemapAndPresent() = default;


    virtual void Execute(vk::CommandBuffer cmd) override;

  private:
    VK::PipelineGraphics pipeline;
};

} // namespace KE::R