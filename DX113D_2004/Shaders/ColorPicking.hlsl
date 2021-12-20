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
    //output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
   
    
    return output;
}

struct PixelOutput
{
    float4 color : SV_Target0; // ������ �� �ε����� rtv�� �Ѵٴ°�.
};



PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    output.color = input.color;
    
    return output;
}