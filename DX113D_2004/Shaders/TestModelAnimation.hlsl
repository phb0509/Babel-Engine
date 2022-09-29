#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 binormal : Binormal;
    float3 viewDir : ViewDir;
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
    
    matrix boneWorld = mul(SkinWorld(input.indices, input.weights), world);
    
    output.pos = mul(input.pos, boneWorld);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
   
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 100.0f;
    return depthValue;
}