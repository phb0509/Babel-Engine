#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 binormal : Binormal;
    float3 worldPos : Position;
    float3 camPos : CamPos;
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
    
    matrix transform = 0;
    
    [flatten]
    if (modelType == 2) // Skinned Mesh
    {
        transform = mul(SkinWorld(input.indices, input.weights), world);
    }
    else if (modelType == 1) // Static Mesh
    {
        transform = mul(BoneWorld(input.indices, input.weights), world);
    }
    else
    {
        transform = world;
    }
       
    output.pos = mul(input.pos, transform);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 13.0f;
    return depthValue;
}