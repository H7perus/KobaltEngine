#pragma once

#include "GpuInterface/Types/DescriptorHandle.h"

#include "glm/glm.hpp"

struct FrameData_t
{
    glm::mat4 ViewPerspectiveTransform;

    float currentTime = 0;
    float timeDelta = 0;

    KE::VK::DescriptorHandle transformBuffers;
};