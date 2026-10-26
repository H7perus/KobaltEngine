// #language 2026

import Engine;


extern static const int SomeVariableForSwitching = 0;




struct VSOutput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VSInput
{
    float3 pos : POSITION; // location 0
    float3 color : COLOR;  // location 1
    float2 uv : TEXCOORD;  // location 2
};


struct MaterialParameters
{
    DescriptorHandle<Sampler2D> albedo;
    DescriptorHandle<Sampler2D> normal;
    DescriptorHandle<Sampler2D> roughness;
};

struct PushConstants
{
    DescriptorHandle<ConstantBuffer<FrameData_t>> FrameData;

    DescriptorHandle<ConstantBuffer<MaterialParameters>> MaterialParams;

    uint transformIndex;
}

[[vk::push_constant]]
PushConstants pc;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    float time = (*pc.FrameData).currentTime;


    float4x3 transform = (*pc.FrameData).transformBuffer[pc.transformIndex];

    float3 translationTest = float3(0, 0, transform[3][2]);

    float3 transformedPos = mul(float4(input.pos, 1.0), transform);

    output.pos   = mul((*pc.FrameData).ViewPerspectiveTransform, float4(transformedPos, 1));
    output.color = input.color;
    output.uv    = input.uv;
    return output;
}


// fragment shader
//[require(spvDescriptorHeapEXT)]
[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{

    DescriptorHandle<Sampler2D> texture = DescriptorHandle<Sampler2D>(uint2(3024, 16));

    int2 pixelCoords = input.pos.xy;

    bool isPink = ((pixelCoords.x / 4 + pixelCoords.y / 4) % 2 == 0);

    return float4(isPink, 0, isPink, 0);
}
