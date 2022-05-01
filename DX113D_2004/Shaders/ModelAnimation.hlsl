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
    
    float3 camPos = invView._41_42_43;
    output.viewDir = normalize(output.pos.xyz - camPos); // DirectionVector from Vertex to LightSource
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.normal = mul(input.normal, (float3x3) boneWorld);
    output.tangent = mul(input.tangent, (float3x3) boneWorld);
    output.binormal = cross(output.normal, output.tangent);
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float4 sampledDiffuseMap = float4(1, 1, 1, 1);
    
    if (hasDiffuseMap)
        sampledDiffuseMap = diffuseMap.Sample(samp, input.uv);
    
    float3 light = normalize(lights[0].direction);
    
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.binormal);
    float3 N = normalize(input.normal);
    
    float3 normal = N;
    
    if (hasNormalMap)
    {
        float4 sampledNormal = normalMap.Sample(samp, input.uv);
    
        float3x3 TBN = float3x3(T, B, N);
        normal = sampledNormal * 2.0f - 1.0f; // 0 ~ 1 Á¤±ÔÈ­.
        normal = normalize(mul(normal, TBN));
    }
    
    float3 viewDir = normalize(input.viewDir);
    float diffuseIntensity = saturate(dot(normal, -light));
    float4 specular = 0;
    
    if (diffuseIntensity > 0)
    {
        float3 halfWay = normalize(viewDir + light);
        specular = saturate(dot(-halfWay, normal));
        
        float4 sampledSpecularMap = float4(1, 1, 1, 1);
        
        if (hasSpecularMap)
        {
            sampledSpecularMap = specularMap.Sample(samp, input.uv);
        }
           
        specular = pow(specular, shininess) * sampledSpecularMap;
    }
    
    float4 diffuse = sampledDiffuseMap * diffuseIntensity * mDiffuse;
    specular *= mSpecular;
    float4 ambient = sampledDiffuseMap * mAmbient;
    
    return diffuse + specular + ambient;
}