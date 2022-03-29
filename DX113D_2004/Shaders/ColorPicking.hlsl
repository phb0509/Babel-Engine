#include "Header.hlsli"

cbuffer ColorBuffer : register(b10)
{
    float4 color;
}

struct PixelInput
{
    float4 pos : SV_Position; 
    float4 color : COLOR;
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
      
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.color = color;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target0
{
    return input.color;
}