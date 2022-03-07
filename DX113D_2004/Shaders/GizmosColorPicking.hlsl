#include "Header.hlsli"


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
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.colorForPicking = input.colorForPicking;
    
    return output;
}

struct PixelOutput
{
    float4 colorForPicking;
};

PixelOutput PS(PixelInput input) : SV_Target1
{
    PixelOutput output;
  
    output.colorForPicking = input.colorForPicking;
    
    return output;
}