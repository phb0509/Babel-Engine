#include "Header.hlsli"



struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

static const float2 arrBasePos[4] = // 2���� NDC���� ��������ǥ.
{
    float2(-1.0f, +1.0f),
    float2(+1.0f, +1.0f),
    float2(-1.0f, -1.0f),
    float2(+1.0f, -1.0f)
};

PixelInput VS(uint vertexID : SV_VertexID) // ����. �� �״�� PS�� �ѱ�� �� �ƴ�.
{
    PixelInput output;
    output.pos = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f); // z�� 0, w�� 1
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
};

SurfaceData UnpackGBuffer(int2 location) // �ȼ���ģ��, ��ǻ� ���ؽ�uv��ǥ�ε�
{
    SurfaceData output;
    
    int3 location3 = int3(location, 0);
    
    float depth = depthTexture.Load(location3).x; // ���̹��۰� ������.
    output.linearDepth = ConvertDepthToLinear(depth); 
    
    float4 diffuse = diffuseTexture.Load(location3);
    
    output.color = diffuse.rgb;
    output.specInt = diffuse.w;
    
    output.normal = normalTexture.Load(location3).xyz;
    output.normal = normalize(output.normal * 2.0f - 1.0f); // -1~1 ����ȭ.
    
    float specular = specularTexture.Load(location3).x;
    output.specPow = specPowerRange.x + specular * specPowerRange.y;
    
    return output;
}

float3 CalcWorldPos(float2 csPos, float linearDepth) // 2d���� �ȼ��� ������ġ���ϱ�.(�����ÿ����ؾ��ϴϱ�)
{
    float4 position;
    
    // UnProjection ����.
    float2 temp;
    temp.x = 1 / pProjection._11; // x�� ����ȭ��.
    temp.y = 1 / pProjection._22; // y�� ����ȭ��.
    position.xy = csPos.xy * temp * linearDepth; // ��ũ��âuv��ǥ * UnProjection x,y������Ʈ * linearDepth
    position.z = linearDepth; // ī�޶�� �ȼ������� �Ÿ�.(������� �Ÿ�)
    position.w = 1.0f;
    
    // ����� ��ǥ�� ��ȯ�Ϸ�.
    
    return mul(position, pInvView).xyz; // ����� ����� �����༭ ���尪 ����.
}

float4 PS(PixelInput input) : SV_Target
{
    SurfaceData data = UnpackGBuffer(input.pos.xy);
        
    
    //float4 temp1 = float4(data.color, data.specInt);
    //float4 temp2 = float4(0.0f, 1.0f, 0.0f, 0.0f);
    //if (temp1 == temp2)
    //{
    //    return float4(0, 1, 0, 1);
    //}
    if (data.color.g >= 0.9f)
    {
        if (data.specInt <= 0.0001f)
        {
            return float4(data.color, data.specInt);
        }
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