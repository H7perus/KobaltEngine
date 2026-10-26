#pragma once

#include "GpuInterface/Types/TaskNode.h"

#include "vulkan/vulkan.hpp"

#include "RendererDLL.h"

namespace KE::VK
{
    class RENDERER_DLL_API  OpaquePass : public VK::ITaskNode
    {
    public:
      OpaquePass();
      ~OpaquePass() = default;

      virtual void Execute(vk::CommandBuffer cmd) override;
    };
    
}