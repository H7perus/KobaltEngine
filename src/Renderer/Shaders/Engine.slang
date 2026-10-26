
public struct FrameData_t
{
    
    public float4x4     ViewPerspectiveTransform;

    public float currentTime;
    public float timeDelta;

    public DescriptorHandle<StructuredBuffer<float4x3>> transformBuffer;
};

[__AttributeUsage(_AttributeTargets.Var)]
struct ConstantRange
{
    int min;
    int max;
};

