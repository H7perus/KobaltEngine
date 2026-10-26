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
    DescriptorHandle<Sampler2D> texture = DescriptorHandle<Sampler2D>(uint2(0, 0)); //TEMP: 0,0 for Intel (no reserved heap range). Was 3024, 16 on NV. //DescriptorHandle<Sampler2D>(uint2(framebufferTextureIndex, 0));

    return texture.SampleLevel(input.uv, 0);
    //return float4(input.uv, 0, 1);
}
