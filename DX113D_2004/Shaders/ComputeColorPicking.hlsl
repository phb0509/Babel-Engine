
cbuffer MouseUV : register(b1)
{
    float2 mouseScreenPosition;
    float2 padding;
}


struct OutputDesc
{
    float4 color;
};


RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D<float4> Texture : register(t0);

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState samp : register(s0);

[numthreads(1, 1, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
    // ���콺��ŷ���� �÷��� ���
    float2 mousePosition;
    mousePosition.x = mouseScreenPosition.x;
    mousePosition.y = mouseScreenPosition.y;
    
    float4 color;
    
    //float r = Texture.GatherRed(samp, mousePosition);
    //float g = Texture.GatherGreen(samp, mousePosition);
    //float b = Texture.GatherBlue(samp, mousePosition);
    //float a = 1.0f;
    
    //color = float4(r, g, b, a);
    
    
   output[0].color = Texture.SampleLevel(samp, mousePosition, 0.0f); // 0~1�� ����ȭ�� ���콺��ǥ�� ���ø�.
    //output[0].color = color;
    

   
    //return diffuseMap.Sample(samp, input.uv) * mDiffuse;
   
}

