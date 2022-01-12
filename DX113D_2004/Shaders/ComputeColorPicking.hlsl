
cbuffer MouseUV : register(b1)
{
    float2 mouseScreenUVPosition;
    int2 mouseScreenPosition;
}


struct OutputDesc
{
    float4 color;
};


RWStructuredBuffer<OutputDesc> output; // CPU로 보낼거.
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
    // 마우스피킹지점 컬러값 얻기
    float2 mouseUVPosition;
    mouseUVPosition.x = mouseScreenUVPosition.x;
    mouseUVPosition.y = mouseScreenUVPosition.y;
    
    int2 mousePosition;
    mousePosition.x = mouseScreenPosition.x;
    mousePosition.y = mouseScreenPosition.y;
    
    //float4 color = Texture.SampleLevel(samp, mouseUVPosition, 0.0f);
    
    float4 color = Texture.Load(int3(mousePosition.xy, 0));
    
    
    
    color.a = 1.0f; // 안해도 피킹하는데엔 상관없지만 영상으로 보여주기위해서..
    output[0].color = color;
}

