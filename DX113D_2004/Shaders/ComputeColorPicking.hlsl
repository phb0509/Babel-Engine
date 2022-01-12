
cbuffer MouseUV : register(b1)
{
    float2 mouseScreenUVPosition;
    int2 mouseScreenPosition;
}


struct OutputDesc
{
    float4 color;
};


RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D<float4> Texture : register(t0);

//SamplerState LinearSampler
//{
//    Filter = MIN_MAG_MIP_LINEAR;

//    AddressU = Clamp;
//    AddressV = Clamp;
//};

//SamplerState samp : register(s0);

[numthreads(1, 1, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
    // ���콺��ŷ���� �÷��� ���
    float2 mouseUVPosition;
    mouseUVPosition.x = mouseScreenUVPosition.x;
    mouseUVPosition.y = mouseScreenUVPosition.y;
    
    int2 mousePosition;
    mousePosition.x = mouseScreenPosition.x;
    mousePosition.y = mouseScreenPosition.y;
    
    //float4 color = Texture.SampleLevel(samp, mouseUVPosition, 0.0f);
    
    float4 color = Texture.Load(int3(mousePosition.xy, 0));
    
    
    
    color.a = 1.0f; // ���ص� ��ŷ�ϴµ��� ��������� �������� �����ֱ����ؼ�..
    output[0].color = color;
}

