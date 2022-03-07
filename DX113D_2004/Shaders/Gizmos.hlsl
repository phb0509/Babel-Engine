#include "Header.hlsli"


cbuffer Color : register(b9)
{
    float4 gizmoColor;
}

struct PixelInput
{
    float4 pos : SV_Position;
    float4 color : Color;
    float4 colorForPicking : ColorForPicking;
};

PixelInput VS(VertexColor input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.color = gizmoColor;
    output.colorForPicking = input.colorForPicking;
    
    return output;
}


struct PixelOutput
{
    float4 color;
};

PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    output.color = input.color;
    return output;
}