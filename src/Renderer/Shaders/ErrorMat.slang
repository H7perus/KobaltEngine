// #language 2026

import Engine;

[ConstantRange(0, 1)]
extern static const int isRed = 0;

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
    int    test : TESTINT;
    float  test2 : TESTFLOAT;
};

[shader("vertex")]
float4 vertMain(VSInput input) : SV_Position
{
    float4 output;

    float4x3 transform = (*pc.FrameData).transformBuffer[pc.transformIndex];

    float3 transformedPos = mul(float4(input.pos, 1.0), transform);

    output   = mul((*pc.FrameData).ViewPerspectiveTransform, float4(transformedPos, 1));
    return output;
}

[shader("fragment")]
float4 fragMain(float4 pos : SV_Position) : SV_Target
{
    uint2 posInt = pos.xy;

    bool isPink = ((posInt.x / 4 + posInt.y / 4) % 2 == 0);

    if (isRed == 1)
    {
        return float4(isPink, 0, 0, 1);
    }

    return float4(isPink, 0, isPink, 1);
}
