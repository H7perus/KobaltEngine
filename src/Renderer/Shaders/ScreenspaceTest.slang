#language 2026

import Engine;

#include "ScreenspaceVert.slang"

typedef uint32_t u32;


struct PushConstants
{
    DescriptorHandle<ConstantBuffer<FrameData_t>> FrameData;
}

[[vk::push_constant]]
PushConstants pc;

[shader("fragment")]
float4 fragMain(VSOutput input)
{
    DescriptorHandle<Sampler2D> texture = DescriptorHandle<Sampler2D>(uint2(3024, 16)); //DescriptorHandle<Sampler2D>(uint2(framebufferTextureIndex, 0));

    return texture.SampleLevel(input.uv, 0);
    //return float4(input.uv, 0, 1);
}
