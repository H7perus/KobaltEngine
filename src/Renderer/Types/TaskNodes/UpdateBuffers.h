#pragma once

#include "GpuInterface/Types/TaskNode.h"

#include "GpuInterface/Types/PipelineGraphics.h"

#include "vulkan/vulkan.hpp"

#include "RendererDLL.h"

namespace KE::R
{
class RENDERER_DLL_API UpdateBuffers : public VK::ITaskNode
{
  public:
    UpdateBuffers() = default;
    ~UpdateBuffers() = default;


    virtual void Execute(vk::CommandBuffer cmd) override;
};

} // namespace KE::R