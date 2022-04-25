#include "Header.hlsli"

cbuffer View : register(b3)
{
    matrix Pview;
    matrix PinvView;
}

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

static const float2 arrBasePos[4] = // 2���� NDC���� ��������ǥ.
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(-1.0f, -1.0f),
    float2(1.0f, -1.0f)
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
};

PixelInput VS(uint vertexID : SV_VertexID) // ����. �� �״�� PS�� �ѱ�� �� �ƴ�.
{
    PixelInput output;
    output.pos = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f); // z�� 0, w�� 1
    output.uv = output.pos.xy;
    
    return output;
}

float ConvertDepthToLinear(float depth)  
{
    return depth;
    //float linearDepth = projection._43 / (depth - projection._33); // 43,33�� ������� z�� Ŭ���ΰ��� ��ȯ�� ���Ѱ�.
    //return linearDepth;
    //return 0.01f;
    //return 0.01f;
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
    //float depth = asfloat(depthTexture.SampleLevel(LinearSampler, location, 0.0f)); // 0~1�� ����ȭ�� ���콺��ǥ�� ���ø�.
    
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

float3 CalcWorldPos(float2 csPos, float depth) // 2d���� �ȼ��� ������ġ���ϱ�.(�����ÿ����ؾ��ϴϱ�)
{
    //float4 position;
    
    //// UnProjection ����.
    //float2 temp;
    //temp.x = 1 / projection._11; // x�� ����ȭ��.
    //temp.y = 1 / projection._22; // y�� ����ȭ��.
    //position.xy = csPos.xy * temp * depth; // ��ũ��âuv��ǥ * UnProjection x,y������Ʈ * linearDepth
    //position.z = depth; // ī�޶�� �ȼ������� �Ÿ�.(������� �Ÿ�)
    //position.w = 1.0f;
    
    //// ����� ��ǥ�� ��ȯ�Ϸ�.
    
    //return mul(position, invView).xyz; // ����� ����� �����༭ ���尪 ����.
    
    
    float4 clipSpacePosition = float4(csPos.x, csPos.y, depth, 1.0f);
    float4 viewSpacePosition = mul(clipSpacePosition, invProjection);
    
    viewSpacePosition /= viewSpacePosition.w;
    float4 worldSpacePosition = mul(viewSpacePosition, invView);
    
    return worldSpacePosition.xyz;
}

float4 PS(PixelInput input) : SV_Target
{
    SurfaceData data = UnpackGBuffer(input.pos.xy);
    
    Material material;
    material.normal = data.normal;
    material.diffuseColor = float4(data.color, 1.0f); // ���۵�Ư���� ���İ� ������ 1�� ����.
    material.camPos = PinvView._41_42_43;
    material.shininess = data.specPow;
    material.specularIntensity = data.specInt.xxxx;
    material.worldPos = CalcWorldPos(input.uv, data.linearDepth);
    material.emissive = float4(0, 0, 0, 0);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float emissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //float4 ambient = CalcAmbient(material) * mAmbient;
    //float4 emissive = CalcEmissive(material);
    
    float4 result = 0;
    
    for (int i = 0; i < lightCount; i++)
    {
        if (!lights[i].active)
            continue;
        
        [flatten]
        if (lights[i].type == 0)
            result += CalcDirectional(material, lights[i]); // ����Ʈ �𷺼Ǹ� ����
        //else if (lights[i].type == 1)
        //    result += CalcPoint(material, lights[i]); // ����
        //else if (lights[i].type == 2)
        //    result += CalcSpot(material, lights[i]); // ������
        //else if (lights[i].type == 3)
        //    result += CalcCapsule(material, lights[i]); //
    }
    
    return result + ambient + emissive;
}