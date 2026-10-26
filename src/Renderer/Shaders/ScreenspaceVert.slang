
struct VSOutput {
    float2 uv : TEXCOORD0;
    float4 position : SV_Position;
};

[shader("vertex")]
VSOutput vertMain(uint vertexID: SV_VulkanVertexID) {
    VSOutput output;
    output.uv       = float2((vertexID << 1) & 2, vertexID & 2);
    output.position = float4(output.uv * 2.0 - 1.0, 0.0, 1.0);
    return output;
}