
cbuffer MouseUV : register(b0)
{
    float2 mouseScreenPosition;
    float2 mouseNDCPosition;
    matrix invViewMatrix;
    matrix projectionMatrix;
}


struct OutputDesc
{
    float outputU;
    float outputV;
    float depthRedValue;
    float padding3;
    
    float3 worldPosition;
    float padding1;
};


RWStructuredBuffer<OutputDesc> output; // CPU로 보낼거.
Texture2D<float> depthTexture : register(t0);
//Texture2D<float> testTexture : register(t1);

float ConvertDepthToLinear(float depth) // 
{
    float linearDepth = projectionMatrix._43 / (depth - projectionMatrix._33); // 43,33은 뷰공간의 z값 클리핑공간 전환을 위한값.
    
    return linearDepth;
}

float3 CalcWorldPos(float2 mouseNDCPosition, float linearDepth) // 2d상의 픽셀의 월드위치구하기.(라이팅연산해야하니까) // -1~1로 정규화해야한다.
{
    float4 position;
    
    float2 temp;
    temp.x = 1 / projectionMatrix._11; // 투영행렬의 _11,_22는 '뷰좌표' -> '-1~1값' 으로의 정규화용 컴포넌트.
    temp.y = 1 / projectionMatrix._22; 
    position.xy = mouseNDCPosition.xy * temp * linearDepth; // UnProjection 과정.
    position.z = linearDepth;
    position.w = 1.0f;
    
    // 그러면 지금 position좌표는 뷰공간의 좌표라는 의미.
    
    return mul(position, invViewMatrix).xyz; // 뷰역행렬 곱해줌으로써 월드좌표 변환. 
}

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
};

[numthreads(1, 1, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
   // int3 location3 = int3(mouseUV, 0);
    
    output[0].outputU = mouseNDCPosition.x;
    output[0].outputV = mouseNDCPosition.y;
    
    float2 screenCoord;
    screenCoord.x = mouseScreenPosition.x;
    screenCoord.y = mouseScreenPosition.y;
    
    float2 ndcCoord;
    ndcCoord.x = mouseNDCPosition.x;
    ndcCoord.y = mouseNDCPosition.y;
     
    output[0].depthRedValue = asfloat(depthTexture.SampleLevel(LinearSampler, screenCoord, 0.0f));
    
    
    
    float depth = asfloat(depthTexture.SampleLevel(LinearSampler, screenCoord, 0.0f)); // 0~1로 정규화한 마우스좌표로 샘플링.
    
    // ConvertDepthToLinear
    float linearDepth = projectionMatrix._43 / (depth - projectionMatrix._33); 
  
    // CalcWorldPos
    float4 position;
    float2 temp;
    temp.x = 1 / projectionMatrix._11; // 투영행렬의 _11,_22는 뷰좌표의 x,y값 정규화시키는 컴포넌트. 나눴으니 UnProjection 과정.
    temp.y = 1 / projectionMatrix._22; // 
    position.xy = mouseNDCPosition.xy * temp * linearDepth; 
    position.z = linearDepth;
    position.w = 1.0f;
    
    float3 worldPosition =  mul(position, invViewMatrix).xyz; // 뷰역행렬 곱해줌으로써 월드좌표 변환. 
    
    
    //float3 worldPosition = CalcWorldPos(ndcCoord, linearDepth);
    output[0].worldPosition = worldPosition;
}

