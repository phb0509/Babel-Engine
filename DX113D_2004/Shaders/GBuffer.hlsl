#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewPos : POSITION1;
    float4 worldPos : POSITION2;
    float4 lightViewPosition : LIGHTVIEWPOSITION;
};

cbuffer ShadowMappingLightBuffer : register(b9)
{
    Matrix lightViewMatrix;
    Matrix lightProjectionMatrix;
}

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
    
    matrix transform = 0;
    
    [flatten]
    if (modelType == 2)
        transform = mul(SkinWorld(input.indices, input.weights), world);
    else if (modelType == 1)
        transform = mul(BoneWorld(input.indices, input.weights), world);
    else
        transform = world;
    
    output.pos = mul(input.pos, transform);
    output.viewPos = invView._41_42_43;
    
    // 광원위치기준 클립스페이스 좌표구하기 (wvp곱)
    output.worldPos = float4(output.pos.xyz, 1.0f);
    output.lightViewPosition = mul(output.worldPos, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);
    
    
    
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.normal = normalize(mul(input.normal, (float3x3) transform));
    output.tangent = normalize(mul(input.tangent, (float3x3) transform));
    output.binormal = cross(output.normal, output.tangent);
    output.uv = input.uv;
    
    return output;
}

struct PixelOutput
{
    float4 diffuse : SV_Target0; 
    float4 specular : SV_Target1;
    float4 normal : SV_Target2;
    float4 depth : SV_Target3; // UITexture용
    float4 specaulrForShow : SV_Target4;
    //float4 worldPosition : SV_Target5;
};

PixelOutput PackGBuffer(PixelInput input, float3 sampledDiffuseMap, float3 mappingedNormal, float sampledSpecularMap) // 디퓨즈값, 스페큘라값,노말값 등 다 계산해놓기.
{
    PixelOutput output;
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 13.0f;
    
    float specPowNorm = (shininess - specPowerRange.x) / (specPowerRange.y); // 스페큘라값 정규화.
    
    output.diffuse = float4(sampledDiffuseMap, sampledSpecularMap); // specIntensity = 텍스쳐에서 샘플링한값.
    output.specular = float4(specPowNorm, 0.0f, 0.0f, 1.0f);
    output.normal = float4(mappingedNormal * 0.5f + 0.5f, 1.0f); // 0~1 정규화.
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f); // UITexture에 렌더할 시연용렌더타겟.
    output.specaulrForShow = float4(0.0f, 0.0f, 0.0f, 1.0f); // UITexture에 렌더할 시연용렌더타겟.
    //output.worldPosition = input.worldPos;
    
    [flatten]
    if (hasSpecularMap)
    {
        output.specaulrForShow = specularMap.Sample(samp, input.uv);
    }
    
    return output;
}

Texture2D depthMapTexture : register(t8);


float3 shadowMapping(PixelInput input, float3 diffuseMap)
{
    float bias = 0.0000005f;
    float2 projectTexCoord;
    
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
    
    float currentDepth = input.lightViewPosition.z / input.lightViewPosition.w;
    float shadowDepth = depthMapTexture.Sample(LinearSampler, projectTexCoord).r; // DSV에 바인딩되어있는 깊이맵SRV

    
    if (currentDepth > shadowDepth + bias)
    {
        diffuseMap *= 0.3f;
    }
    
    return diffuseMap;
}

PixelOutput PS(PixelInput input) : SV_Target
{
    float3 sampledDiffuseMap = float3(1.0f, 1.0f, 1.0f);
    
    [flatten]
    if (hasDiffuseMap)
    {
        sampledDiffuseMap = diffuseMap.Sample(samp, input.uv).rgb;
    }
      
    sampledDiffuseMap = shadowMapping(input, sampledDiffuseMap);
    
    float sampledSpecularMap = 1.0f;
    
    [flatten]
    if (hasSpecularMap)
    {
        sampledSpecularMap = specularMap.Sample(samp, input.uv).r;
    }
        
    float3 mappingedNormal = NormalMapping(input.tangent, input.binormal, input.normal, input.uv);
    
    return PackGBuffer(input, sampledDiffuseMap, mappingedNormal, sampledSpecularMap); // 
}