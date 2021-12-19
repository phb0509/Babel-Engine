
cbuffer MouseUV : register(b0)
{
    float2 mouseScreenPosition;
    float2 padding;
}


struct OutputDesc
{
    float4 color;
};


RWStructuredBuffer<OutputDesc> output; // CPU�� ������.
Texture2D<float> Texture : register(t0);

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
};

[numthreads(1, 1, 1)]
void CS(uint3 index : SV_DispatchThreadID)
{
    // ���콺��ŷ���� �÷��� ���
    //output[0].color = Texture.SampleLevel(LinearSampler, mouseScreenPosition, 0.0f); // 0~1�� ����ȭ�� ���콺��ǥ�� ���ø�.
    output[0].color = float4(2.0f, 2.0f, 1.0f, 1.0f);
    //return diffuseMap.Sample(samp, input.uv) * mDiffuse;
   
}

