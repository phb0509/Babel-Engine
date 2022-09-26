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
    if(modelType == 2) // Skinned Mesh
    {
        transform = mul(SkinWorld(input.indices, input.weights), world);
    }
    else if(modelType == 1) // Static Mesh
    {
        transform = mul(BoneWorld(input.indices, input.weights), world);
    }
    else
    {
        transform = world;
    }
       
    output.pos = mul(input.pos, transform);
    
    output.camPos = invView._41_42_43; 
    output.worldPos = output.pos;
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.normal = mul(input.normal, (float3x3) transform);
    output.tangent = mul(input.tangent, (float3x3) transform);
    output.binormal = cross(output.normal, output.tangent);
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float4 sampledDiffuseMap = float4(1, 1, 1, 1);
    
    if (hasDiffuseMap)
        sampledDiffuseMap = diffuseMap.Sample(samp, input.uv);
    
    Material material;
    material.normal = NormalMapping(input.tangent, input.binormal, input.normal, input.uv);
    material.diffuseColor = sampledDiffuseMap;
    material.camPos = input.camPos;
    material.emissive = mEmissive;
    material.shininess = shininess;
    material.specularIntensity = SpecularMapping(input.uv);
    material.worldPos = input.worldPos;    
    
    float4 result = CalcLights(material);
    float4 ambient = CalcAmbient(material) * mAmbient;
    float4 emissive = CalcEmissive(material);
    
    return result + ambient + emissive;
}