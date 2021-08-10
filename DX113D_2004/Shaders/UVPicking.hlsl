
cbuffer MouseUV : register(b0)
{
    float2 mouseUV;
    matrix invView;
    matrix projection;
}

struct InputDesc // ���������.
{
    uint index;
    float3 v0;
    float3 v1;
    float3 v2;
};


struct OutputDesc
{
    int picked;
    float u;
    float v;
    float distance;
};

StructuredBuffer<InputDesc> input; // CPU���� �о����.
RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D depthTexture : register(t1);


float ConvertDepthToLinear(float depth) // 
{
    float linearDepth = projection._43 / (depth - projection._33); // 43,33�� ������� z�� Ŭ���ΰ��� ��ȯ�� ���Ѱ�.
    
    return linearDepth;
}

float3 CalcWorldPos(float2 mouseUVPosition, float linearDepth) // 2d���� �ȼ��� ������ġ���ϱ�.(�����ÿ����ؾ��ϴϱ�)
{
    float4 position;
    
    float2 temp;
    temp.x = 1 / projection._11; // x�� ����ȭ��.
    temp.y = 1 / projection._22; // y�� ����ȭ��.
    position.xy = mouseUVPosition.xy * temp * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    
    return mul(position, invView).xyz; // �俪��� ���������ν� ������ǥ ��ȯ. 
}



[numthreads(32, 32, 1)]
void CS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint index = groupID.x + groupIndex; // ��Ȯ�� �𸣰����� �̷����ϸ� �ε����� 0���� �����ؼ� ��~ +1��
    
    int3 location3 = int3(mouseUV, 0);
 
    float depth = depthTexture.Load(location3).x;
    float linearDepth = ConvertDepthToLinear(depth);
    
    float3 worldPos = CalcWorldPos(mouseUV, linearDepth);
    
    output[index].picked = 1;
    output[index].u = worldPos.x;
    output[index].v = worldPos.y;
    output[index].distance = worldPos.z;
    
    //output[index].picked = 1;
    //output[index].u = mouseUV.x + 0.2f;
    //output[index].v = mouseUV.y + 0.2f;
    //output[index].distance = mouseUV.x;
   
}

