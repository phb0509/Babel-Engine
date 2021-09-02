
cbuffer MouseUV : register(b0)
{
    float2 mouseUV;
    float padding1;
    float padding2;
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

float3 CalcWorldPos(float2 mouseUVPosition, float linearDepth) // 2d상의 픽셀의 월드위치구하기.(라이팅연산해야하니까)
{
    float4 position;
    
    float2 temp;
    temp.x = 1 / projectionMatrix._11; // x값 정규화용.
    temp.y = 1 / projectionMatrix._22; // y값 정규화용.
    position.xy = mouseUVPosition.xy * temp * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    
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
    
    output[0].outputU = mouseUV.x;
    output[0].outputV = mouseUV.y;
    
    float2 coord;
    coord.x = mouseUV.x;
    coord.y = mouseUV.y;
    
    output[0].depthRedValue = asfloat(depthTexture.SampleLevel(LinearSampler, coord, 0.0f));
        
    float depth = output[0].depthRedValue;
    float linearDepth = ConvertDepthToLinear(depth);
    float3 worldPosition = CalcWorldPos(coord, linearDepth);
    
    output[0].worldPosition = worldPosition;
}

