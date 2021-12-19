
cbuffer MouseUV : register(b0)
{
    float2 mouseScreenPosition;
    float2 padding;
}


struct OutputDesc
{
    float4 color;
};


RWStructuredBuffer<OutputDesc> output; // CPU로 보낼거.
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
    // 마우스피킹지점 컬러값 얻기
    //output[0].color = Texture.SampleLevel(LinearSampler, mouseScreenPosition, 0.0f); // 0~1로 정규화한 마우스좌표로 샘플링.
    output[0].color = float4(2.0f, 2.0f, 1.0f, 1.0f);
    //return diffuseMap.Sample(samp, input.uv) * mDiffuse;
   
}

