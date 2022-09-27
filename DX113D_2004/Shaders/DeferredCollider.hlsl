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
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
    //float4 normal : SV_Target2;
    //float4 depth : SV_Target3; // UITexture¿ë
    //float4 specaulrForShow : SV_Target4;
};


PixelOutput PS(PixelInput input) : SV_Target0
{
    PixelOutput output;
    output.diffuse = mDiffuse;
    output.specular = float4(0.0f, 1.0f, 0.0f, 0.0f);
    return output;
}