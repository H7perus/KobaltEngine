// #language 2026

import Engine;



struct PushConstants
{
    DescriptorHandle<ConstantBuffer<FrameData_t>>        FrameData;
    uint                                          transformIndex;
}




[[vk::push_constant]]
PushConstants pc;

struct VSInput
{
    float3 pos : POSITION; // location 0
    float3 color : COLOR;  // location 1
    float2 uv : TEXCOORD;  // location 2
    float3 normal : NORMAL;
    int    test : TESTINT;
    float  test2 : TESTFLOAT;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv;
    float3 normal;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    float4x3 transform = (*pc.FrameData).transformBuffer[pc.transformIndex];

    float3 transformedPos = mul(float4(input.pos, 1.0), transform);

    output.pos = mul((*pc.FrameData).ViewPerspectiveTransform, float4(transformedPos, 1));
    output.uv     = input.uv;
    output.normal = input.normal;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
    return input.uv.xyxy;
}
