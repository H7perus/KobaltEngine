#pragma once

#include "BasicTypeAliases.h"

namespace KE::VK
{
    //Represents a slang descriptor handle type. Both textures and buffers will contain a resource index, while textures also contain a sampler index.

    class DescriptorHandle
    {
        u32 resourceIndex_;
        u32 samplerIndex_;
    public:
        DescriptorHandle(u32 resourceIndex, u32 samplerIndex)
            : resourceIndex_(resourceIndex), samplerIndex_(samplerIndex) {}

        
    };

}