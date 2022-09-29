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

PixelInput VS(uint vertexID : SV_VertexID) // 주의. 값 그대로 PS에 넘기는 것 아님.
{
    PixelInput output;
    output.pos = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f); // z값 0, w값 1
    output.uv = output.pos.xy;
    
    return output;
}

cbuffer ViewBuffer : register(b10)
{
    matrix pView;
    matrix pInvView;
}

cbuffer ProjectionBuffer : register(b11)
{
    matrix pProjection;
}

Texture2D depthTexture : register(t10);
Texture2D diffuseTexture : register(t11);
Texture2D specularTexture : register(t12);
Texture2D normalTexture : register(t13);

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

float3 CalcWorldPos(float2 csPos, float linearDepth) // 2d상의 픽셀의 월드위치구하기.(라이팅연산해야하니까)
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
}

SurfaceData UnpackGBuffer(int2 location) // 픽셀위친데, 사실상 버텍스uv좌표인듯
{
    SurfaceData output;
    
    int3 location3 = int3(location, 0);
    
    float depth = depthTexture.Load(location3).x; // 깊이버퍼값 빼오기.
    output.linearDepth = ConvertDepthToLinear(depth); 
    
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
    
    float4 result = CalcLights(material);
    float4 ambient = CalcAmbient(material);
   
    return result + ambient;
}