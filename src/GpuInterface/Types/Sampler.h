#pragma once

#include "Resource.h"

#include "vulkan/vulkan.hpp"

namespace KE::VK
{
class Sampler : public IResource
{
    KE_REFLECT(Sampler)

    vk::SamplerCreateInfo samplerInfo_;
    u32                   deviceIndex_;
    u32                   samplerHeapIndex_;

  public:
    Sampler() = default;
    Sampler(u32 deviceIndex, bool addToHeap = false);
};
} // namespace KE::VK