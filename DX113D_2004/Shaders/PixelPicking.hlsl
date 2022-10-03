
cbuffer MouseUV : register(b0)
{
    float2 mouseScreenPosition;
    float2 mouseNDCPosition;
    matrix invViewMatrix;
    matrix invProjectionMatrix;
}


struct OutputDesc
{
    float3 worldPosition;
    float padding1;
};

RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D<float> depthTexture : register(t0);

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
};

[numthreads(1, 1, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
    float2 screenCoord; // depth sampling�� ���� ��ũ����ǥ.
    screenCoord.x = mouseScreenPosition.x;
    screenCoord.y = mouseScreenPosition.y;
    
    float2 ndcCoord; // -1 ~ 1�� ����ȭ�� ��.
    ndcCoord.x = mouseNDCPosition.x;
    ndcCoord.y = mouseNDCPosition.y;
     
    //DepthSampling
    float depth = asfloat(depthTexture.SampleLevel(LinearSampler, screenCoord, 0.0f)); // 0~1�� ����ȭ�� ���콺��ǥ�� ���ø�.
    
    //CalcWorldPositin
   
    float4 clipSpacePosition = float4(ndcCoord.x, ndcCoord.y, depth, 1.0f); // 
    float4 viewSpacePosition = mul(clipSpacePosition, invProjectionMatrix); // ��������ؼ� �������ǥ����.

    // Perspective division
     
    viewSpacePosition /= viewSpacePosition.w; // inverse W���̶� �����ִ°�.
                                              // farZ 1000���� �����ϸ� ����ȼ��� �Ⱥ��������̸� W���� 0.001��.100�̸� 0.01�̷���..      

    float4 worldSpacePosition = mul(viewSpacePosition, invViewMatrix);
    
    output[0].worldPosition = worldSpacePosition.xyz;
    output[0].padding1 = depth;
}

