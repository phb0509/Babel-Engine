#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

static const float2 arrBasePos[4] = // 2차원 NDC공간 꼭지점좌표.
{
    float2(-1.0f, +1.0f),
    float2(+1.0f, +1.0f),
    float2(-1.0f, -1.0f),
    float2(+1.0f, -1.0f)
};

PixelInput VS(uint vertexID : SV_VertexID) 
{
    PixelInput output;
    output.pos = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f); // z값 0, w값 1
    output.uv = output.pos.xy;
    
    return output;
}

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

cbuffer ViewBuffer : register(b10)
{
    matrix pView;
    matrix pInvView;
}

cbuffer ProjectionBuffer : register(b11)
{
    matrix pProjection;
    matrix pInvProjection;
}

cbuffer ShadowMappingLightBuffer : register(b9)
{
    Matrix lightViewMatrix;
    Matrix lightProjectionMatrix;
}

Texture2D shadowMapTexture : register(t8);
Texture2D depthTexture : register(t10);
Texture2D diffuseTexture : register(t11);
Texture2D specularTexture : register(t12);
Texture2D normalTexture : register(t13);
Texture2D worldTexture : register(t14);

float ConvertDepthToLinear(float depth)  
{
    float linearDepth = pProjection._43 / (depth - pProjection._33);
    return linearDepth;
}

struct SurfaceData
{
    float linearDepth;
    float3 color;
    float3 normal;
    float specInt;
    float specPow;
    float isCollider;
};

float3 CalcWorldPos(float2 csPos, float linearDepth) // csPos == input.uv
{
    float4 position;
    
    // UnProjection 과정.
    float2 temp;
    temp.x = 1 / pProjection._11; // x값 정규화용.
    temp.y = 1 / pProjection._22; // y값 정규화용.
    position.xy = csPos.xy * temp * linearDepth; // 스크린창uv좌표 * UnProjection x,y컴포넌트 * linearDepth
    position.z = linearDepth; // 카메라와 픽셀사이의 거리.(월드상의 거리)
    position.w = 1.0f;
    
    // 뷰공간 좌표로 변환완료.
    
    return mul(position, pInvView).xyz; // 뷰행렬 역행렬 곱해줘서 월드값 도출.
    
    //float4 clipSpacePosition = float4(csPos.x, csPos.y, linearDepth, 1.0f);
    //float4 viewSpacePosition = mul(clipSpacePosition, pInvProjection);
    //viewSpacePosition /= viewSpacePosition.w;
    
    //float4 worldSpacePosition = mul(viewSpacePosition, pInvView);
    
    //return worldSpacePosition.xyz;
}

SurfaceData UnpackGBuffer(int2 location) // 픽셀위친데, 사실상 버텍스uv좌표인듯
{
    SurfaceData output;
    
    int3 location3 = int3(location, 0.0f);
    
    //float depth = depthTexture.Load(location3).r; // 깊이버퍼값 빼오기.
    //output.linearDepth = ConvertDepthToLinear(depth); 
    
    float depth = asfloat(depthTexture.SampleLevel(LinearSampler, location, 0.0f));
    output.linearDepth = depth; 
    
    float4 diffuse = diffuseTexture.Load(location3);
    
    output.color = diffuse.rgb;
    output.specInt = diffuse.a;
    
    output.normal = normalTexture.Load(location3).rgb;
    output.normal = normalize(output.normal * 2.0f - 1.0f); // -1~1 정규화.
    
    float specular = specularTexture.Load(location3).r;
    output.specPow = specPowerRange.r + specular * specPowerRange.g;
    output.isCollider = specularTexture.Load(location3).g;
    return output;
}

float3 shadowMapping(float3 worldPos, float3 diffuseMap) // worldPos는 제대로 가져온다.
{
    float bias = 0.0000125f;
    float2 projectTexCoord;
    float4 lightViewPosition = float4(worldPos, 1.0f);
    
    lightViewPosition = mul(lightViewPosition, lightViewMatrix);
    lightViewPosition = mul(lightViewPosition, lightProjectionMatrix);
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
    
    float currentDepth = lightViewPosition.z / lightViewPosition.w; // 값이 제대로 안담기는것같다. 
    float shadowDepth = shadowMapTexture.Sample(LinearSampler, projectTexCoord).r; // 
    
    if (currentDepth > shadowDepth + bias)
    {
        diffuseMap *= 0.1f;
    }
    
    if (shadowDepth < 0.9f)
    {
        diffuseMap = float3(1.0f, 0.0f, 0.0f);
    }
    
    if (shadowDepth > 0.9f)
    {
        diffuseMap = float3(0.0f, 1.0f, 0.0f);
    }
    
    if (shadowDepth > 0.925f)
    {
        diffuseMap = float3(0.0f, 0.0f, 1.0f);
    }
   
    

        return diffuseMap;
}

float4 PS(PixelInput input) : SV_Target
{
    SurfaceData data = UnpackGBuffer(input.pos.xy);
    
    if (data.isCollider > 0.0f)
    {
        return float4(data.color, 1.0f);
    }
        
    Material material;
    material.normal = data.normal;
    material.diffuseColor = float4(data.color, 1.0f);
    material.camPos = pInvView._41_42_43;
    material.specularIntensity = float4(data.specInt.xxx, 1.0f);
    material.worldPos = CalcWorldPos(input.uv, data.linearDepth);
    //material.worldPos = worldTexture.Load(int3(input.pos.xy, 0)).xyz;
        
    float4 result = CalcLights(material);
    //result.xyz = shadowMapping(material.worldPos,result.xyz);
    
    float4 ambient = CalcAmbient(material);
    
    return result + ambient;
}