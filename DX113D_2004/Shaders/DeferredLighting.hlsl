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

static const float2 arrBasePos[4] = // 2차원 NDC공간 꼭지점좌표.
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

PixelInput VS(uint vertexID : SV_VertexID) // 주의. 값 그대로 PS에 넘기는 것 아님.
{
    PixelInput output;
    output.pos = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f); // z값 0, w값 1
    output.uv = output.pos.xy;
    
    return output;
}

float ConvertDepthToLinear(float depth)  
{
    return depth;
    //float linearDepth = projection._43 / (depth - projection._33); // 43,33은 뷰공간의 z값 클리핑공간 전환을 위한값.
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

SurfaceData UnpackGBuffer(int2 location) // 픽셀위친데, 사실상 버텍스uv좌표인듯
{
    SurfaceData output;
    
    int3 location3 = int3(location, 0);
    
    float depth = depthTexture.Load(location3).x; // 깊이버퍼값 빼오기.
    //float depth = asfloat(depthTexture.SampleLevel(LinearSampler, location, 0.0f)); // 0~1로 정규화한 마우스좌표로 샘플링.
    
    output.linearDepth = ConvertDepthToLinear(depth); 
    
    float4 diffuse = diffuseTexture.Load(location3);
    
    output.color = diffuse.rgb;
    output.specInt = diffuse.w;
    
    output.normal = normalTexture.Load(location3).xyz;
    output.normal = normalize(output.normal * 2.0f - 1.0f); // -1~1 정규화.
    
    float specular = specularTexture.Load(location3).x;
    output.specPow = specPowerRange.x + specular * specPowerRange.y;
    
    return output;
}

float3 CalcWorldPos(float2 csPos, float depth) // 2d상의 픽셀의 월드위치구하기.(라이팅연산해야하니까)
{
    //float4 position;
    
    //// UnProjection 과정.
    //float2 temp;
    //temp.x = 1 / projection._11; // x값 정규화용.
    //temp.y = 1 / projection._22; // y값 정규화용.
    //position.xy = csPos.xy * temp * depth; // 스크린창uv좌표 * UnProjection x,y컴포넌트 * linearDepth
    //position.z = depth; // 카메라와 픽셀사이의 거리.(월드상의 거리)
    //position.w = 1.0f;
    
    //// 뷰공간 좌표로 변환완료.
    
    //return mul(position, invView).xyz; // 뷰행렬 역행렬 곱해줘서 월드값 도출.
    
    
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
    material.diffuseColor = float4(data.color, 1.0f); // 디퍼드특성상 알파값 못쓰니 1로 고정.
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
            result += CalcDirectional(material, lights[i]); // 라이트 디렉션만 영향
        //else if (lights[i].type == 1)
        //    result += CalcPoint(material, lights[i]); // 전구
        //else if (lights[i].type == 2)
        //    result += CalcSpot(material, lights[i]); // 손전등
        //else if (lights[i].type == 3)
        //    result += CalcCapsule(material, lights[i]); //
    }
    
    return result + ambient + emissive;
}