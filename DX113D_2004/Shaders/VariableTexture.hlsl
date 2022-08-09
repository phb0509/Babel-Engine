#include "Header.hlsli"

cbuffer TextureBuffer : register(b6)
{
    float widthRatio;
    float heightRatio;
    float2 pad;
};

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

PixelInput VS(VertexUV input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float4 color = diffuseMap.Sample(samp, input.uv);

    if (input.uv.x >= widthRatio)
    {
        if (color.a != 0)
        {
            color = float4(0.0f, 0.0f, 0.0f, 1.0f);
        }  
    }

    return color;
}