[[vk::binding(0)]]RWStructuredBuffer<int32_t> outputBuffer;

[shader("compute")]
[numthreads(8, 8, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    outputBuffer[dispatchThreadID.x + 8 * dispatchThreadID.y] = 7 - dispatchThreadID.x;
}
