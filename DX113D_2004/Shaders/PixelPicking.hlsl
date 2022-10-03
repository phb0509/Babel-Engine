
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

RWStructuredBuffer<OutputDesc> output; // CPU로 보낼거.
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
    float2 screenCoord; // depth sampling을 위한 스크린좌표.
    screenCoord.x = mouseScreenPosition.x;
    screenCoord.y = mouseScreenPosition.y;
    
    float2 ndcCoord; // -1 ~ 1로 정규화된 값.
    ndcCoord.x = mouseNDCPosition.x;
    ndcCoord.y = mouseNDCPosition.y;
     
    //DepthSampling
    float depth = asfloat(depthTexture.SampleLevel(LinearSampler, screenCoord, 0.0f)); // 0~1로 정규화한 마우스좌표로 샘플링.
    
    //CalcWorldPositin
   
    float4 clipSpacePosition = float4(ndcCoord.x, ndcCoord.y, depth, 1.0f); // 
    float4 viewSpacePosition = mul(clipSpacePosition, invProjectionMatrix); // 여기까지해서 뷰공간좌표구함.

    // Perspective division
     
    viewSpacePosition /= viewSpacePosition.w; // inverse W값이라 나눠주는것.
                                              // farZ 1000으로 설정하면 모든픽셀이 안보일쯔음이면 W값이 0.001임.100이면 0.01이런식..      

    float4 worldSpacePosition = mul(viewSpacePosition, invViewMatrix);
    
    output[0].worldPosition = worldSpacePosition.xyz;
    output[0].padding1 = depth;
}

