
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


RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D<float> depthTexture : register(t0);
//Texture2D<float> testTexture : register(t1);

float ConvertDepthToLinear(float depth) // 
{
    float linearDepth = projectionMatrix._43 / (depth - projectionMatrix._33); // 43,33�� ������� z�� Ŭ���ΰ��� ��ȯ�� ���Ѱ�.
    
    return linearDepth;
}

float3 CalcWorldPos(float2 mouseNDCPosition, float linearDepth) // 2d���� �ȼ��� ������ġ���ϱ�.(�����ÿ����ؾ��ϴϱ�) // -1~1�� ����ȭ�ؾ��Ѵ�.
{
    float4 position;
    
    float2 temp;
    temp.x = 1 / projectionMatrix._11; // ��������� _11,_22�� '����ǥ' -> '-1~1��' ������ ����ȭ�� ������Ʈ.
    temp.y = 1 / projectionMatrix._22; 
    position.xy = mouseNDCPosition.xy * temp * linearDepth; // UnProjection ����.
    position.z = linearDepth;
    position.w = 1.0f;
    
    // �׷��� ���� position��ǥ�� ������� ��ǥ��� �ǹ�.
    
    return mul(position, invViewMatrix).xyz; // �俪��� ���������ν� ������ǥ ��ȯ. 
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
    
    
    
    float depth = asfloat(depthTexture.SampleLevel(LinearSampler, screenCoord, 0.0f)); // 0~1�� ����ȭ�� ���콺��ǥ�� ���ø�.
    
    // ConvertDepthToLinear
    float linearDepth = projectionMatrix._43 / (depth - projectionMatrix._33); 
  
    // CalcWorldPos
    float4 position;
    float2 temp;
    temp.x = 1 / projectionMatrix._11; // ��������� _11,_22�� ����ǥ�� x,y�� ����ȭ��Ű�� ������Ʈ. �������� UnProjection ����.
    temp.y = 1 / projectionMatrix._22; // 
    position.xy = mouseNDCPosition.xy * temp * linearDepth; 
    position.z = linearDepth;
    position.w = 1.0f;
    
    float3 worldPosition =  mul(position, invViewMatrix).xyz; // �俪��� ���������ν� ������ǥ ��ȯ. 
    
    
    //float3 worldPosition = CalcWorldPos(ndcCoord, linearDepth);
    output[0].worldPosition = worldPosition;
}

