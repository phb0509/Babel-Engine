#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewPos : POSITION1;
    float3 worldPos : POSITION2;
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
    output.worldPos = output.pos;
    
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
};

PixelOutput PackGBuffer(PixelInput input, float3 sampledDiffuseMap, float3 mappingedNormal, float sampledSpecularMap) // 디퓨즈값, 스페큘라값,노말값 등 다 계산해놓기.
{
    PixelOutput output;
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 13.0f;
    
    //float specPowNorm = (mSpecular.a - specPowerRange.x) / specPowerRange.y; // 스페큘라값 정규화.
    float specPowNorm = (shininess - specPowerRange.x) / (specPowerRange.y); // 스페큘라값 정규화.
    
    output.diffuse = float4(sampledDiffuseMap, sampledSpecularMap); // specIntensity = 텍스쳐에서 샘플링한값.
    output.specular = float4(specPowNorm, 0.0f, 0.0f, 1.0f);
    output.normal = float4(mappingedNormal * 0.5f + 0.5f, 1.0f); // 0~1 정규화.
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f); // UITexture에 렌더할 시연용렌더타겟.
    output.specaulrForShow = float4(0.0f, 0.0f, 0.0f, 1.0f); // UITexture에 렌더할 시연용렌더타겟.
    
    [flatten]
    if (hasSpecularMap)
    {
        output.specaulrForShow = specularMap.Sample(samp, input.uv);
    }
    
    return output;
}

PixelOutput PS(PixelInput input) : SV_Target
{
    float3 sampledDiffuseMap = float3(1.0f, 1.0f, 1.0f);
    
    [flatten]
    if (hasDiffuseMap)
    {
        sampledDiffuseMap = diffuseMap.Sample(samp, input.uv).rgb;
    }
      
    float sampledSpecularMap = 1.0f;
    
    [flatten]
    if (hasSpecularMap)
    {
        sampledSpecularMap = specularMap.Sample(samp, input.uv).r;
    }
        
    float3 mappingedNormal = NormalMapping(input.tangent, input.binormal, input.normal, input.uv);
    
    return PackGBuffer(input, sampledDiffuseMap, mappingedNormal, sampledSpecularMap); // 
}