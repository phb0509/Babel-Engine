#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position;    
};

PixelInput VS(Vertex input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);    
    
    return output;
}

struct PixelOutput
{
    float4 color;
};


PixelOutput PS(PixelInput input) : SV_Target0
{
    PixelOutput output;
    //output.color = float4(200, 200, 0, 1);
    output.color = mDiffuse;    
    
    return output;
}