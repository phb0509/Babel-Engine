
cbuffer MouseUV : register(b0)
{
    float2 mouseUV;
    matrix invView;
    matrix projection;
}


struct OutputDesc
{
    int picked;
    float x;
    float y;
    float z;
};


RWStructuredBuffer<OutputDesc> output; // CPU로 보낼거.
Texture2D depthTexture : register(t1);

float ConvertDepthToLinear(float depth) // 
{
    float linearDepth = projection._43 / (depth - projection._33); // 43,33은 뷰공간의 z값 클리핑공간 전환을 위한값.
    
    return linearDepth;
}

float3 CalcWorldPos(float2 mouseUVPosition, float linearDepth) // 2d상의 픽셀의 월드위치구하기.(라이팅연산해야하니까)
{
    float4 position;
    
    float2 temp;
    temp.x = 1 / projection._11; // x값 정규화용.
    temp.y = 1 / projection._22; // y값 정규화용.
    position.xy = mouseUVPosition.xy * temp * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    
    return mul(position, invView).xyz; // 뷰역행렬 곱해줌으로써 월드좌표 변환. 
}

[numthreads(32, 32, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
    //uint index = groupID.x + groupIndex; // 정확힌 모르겠지만 이렇게하면 인덱스에 0부터 대입해서 쭉~ +1씩
    
    int3 location3 = int3(mouseUV, 0);
 
    float depth = depthTexture.Load(location3).x;
    float linearDepth = ConvertDepthToLinear(depth);
    float3 worldPos = CalcWorldPos(mouseUV, linearDepth);
    
    //output[index].picked = 1;
    //output[index].u = worldPos.x;
    //output[index].v = worldPos.y;
    //output[index].distance = worldPos.z;
    
    //output[index].picked = 1;
    //output[index].u = mouseUV.x + 0.2f;
    //output[index].v = mouseUV.y + 0.2f;
    //output[index].distance = mouseUV.x;
    
    //output[index].picked = 1;
    //output[index].x = depthTexture.Load()
    
    //output[index].picked = 1;
    //output[index].x = worldPos.x;
    //output[index].y = worldPos.y;
    //output[index].z = worldPos.z;
   
}

