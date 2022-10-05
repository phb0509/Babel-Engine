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
    
    // ������ġ���� Ŭ�������̽� ��ǥ���ϱ� (wvp��)
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
    float4 depth : SV_Target3; // UITexture��
    float4 specaulrForShow : SV_Target4;
    //float4 worldPosition : SV_Target5;
};

PixelOutput PackGBuffer(PixelInput input, float3 sampledDiffuseMap, float3 mappingedNormal, float sampledSpecularMap) // ��ǻ�, ����ŧ��,�븻�� �� �� ����س���.
{
    PixelOutput output;
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 13.0f;
    
    float specPowNorm = (shininess - specPowerRange.x) / (specPowerRange.y); // ����ŧ�� ����ȭ.
    
    output.diffuse = float4(sampledDiffuseMap, sampledSpecularMap); // specIntensity = �ؽ��Ŀ��� ���ø��Ѱ�.
    output.specular = float4(specPowNorm, 0.0f, 0.0f, 1.0f);
    output.normal = float4(mappingedNormal * 0.5f + 0.5f, 1.0f); // 0~1 ����ȭ.
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f); // UITexture�� ������ �ÿ��뷻��Ÿ��.
    output.specaulrForShow = float4(0.0f, 0.0f, 0.0f, 1.0f); // UITexture�� ������ �ÿ��뷻��Ÿ��.
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
    float shadowDepth = depthMapTexture.Sample(LinearSampler, projectTexCoord).r; // DSV�� ���ε��Ǿ��ִ� ���̸�SRV

    
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