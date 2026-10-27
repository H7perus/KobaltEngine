#language 2026

import Engine;

#include "ScreenspaceVert.slang"

typedef uint32_t u32;


struct PushConstants
{
    DescriptorHandle<Sampler2D> hdrFramebuffer;
}

[[vk::push_constant]]
PushConstants pc;

[shader("fragment")]
float4 fragMain(VSOutput input)
{
    return pc.hdrFramebuffer.SampleLevel(input.uv, 0);
    //return float4(input.uv, 0, 1);
}
