#pragma once

#include "GpuInterface/Types/PipelineGraphics.h"

namespace KE::R
{
    class Material
    {
        std::shared_ptr<VK::PipelineGraphics> pipeline_;
        u32 sbtIndex;
    };
}