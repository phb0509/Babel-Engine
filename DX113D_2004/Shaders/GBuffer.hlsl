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
    float4 depth : SV_Target3; // UITexture��
    float4 specaulrForShow : SV_Target4;
};

PixelOutput PackGBuffer(PixelInput input, float3 sampledDiffuseMap, float3 mappingedNormal, float sampledSpecularMap) // ��ǻ�, ����ŧ��,�븻�� �� �� ����س���.
{
    PixelOutput output;
    float depthValue = input.pos.z / input.pos.w;
    depthValue *= 13.0f;
    
    //float specPowNorm = (mSpecular.a - specPowerRange.x) / specPowerRange.y; // ����ŧ�� ����ȭ.
    float specPowNorm = (shininess - specPowerRange.x) / (specPowerRange.y); // ����ŧ�� ����ȭ.
    
    output.diffuse = float4(sampledDiffuseMap, sampledSpecularMap); // specIntensity = �ؽ��Ŀ��� ���ø��Ѱ�.
    output.specular = float4(specPowNorm, 0.0f, 0.0f, 1.0f);
    output.normal = float4(mappingedNormal * 0.5f + 0.5f, 1.0f); // 0~1 ����ȭ.
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f); // UITexture�� ������ �ÿ��뷻��Ÿ��.
    output.specaulrForShow = float4(0.0f, 0.0f, 0.0f, 1.0f); // UITexture�� ������ �ÿ��뷻��Ÿ��.
    
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