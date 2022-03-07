#include "Header.hlsli"


cbuffer Brush : register(b10)
{
    int type;
    float3 location; // 마우스피킹 포지션.
    float range;
    float3 color;
}

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    
    float3 binormal : Binormal;
    float3 viewDir : ViewDir;
    float3 worldPos : Position;
    float4 alpha : Alpha;
};

float3 BrushColor(float3 pos) // 픽셀위치.
{
    if (type == 0) // 원
    {
        float x = pos.x - location.x; // 픽셀위치 - 커서위치
        float z = pos.z - location.z;
        
        float distance = sqrt(x * x + z * z);
        
        if (distance <= range)
            return color;
    }
    
    if (type == 1) // 사각형
    {
        if (pos.x >= location.x - range &&
            pos.x <= location.x + range &&
            pos.z >= location.z - range &&
            pos.z <= location.z + range)
        {
            return color;
        }
    }
    
    return float3(0, 0, 0);
}

PixelInput VS(VertexUVNormalTangentAlpha input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    
    float3 camPos = invView._41_42_43;
    output.viewDir = normalize(output.pos.xyz - camPos);
    
    output.worldPos = output.pos;
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
   
    output.normal = mul(input.normal, (float3x3) world);
    output.tangent = mul(input.tangent, (float3x3) world);
    output.binormal = cross(output.normal, output.tangent);
    
    output.uv = input.uv;
    output.alpha = input.alpha;
    
    return output;
}


Texture2D Layers[4] : register(t11);
//Texture2D thirdMap : register(t12);
//Texture2DArray arrays : register(t12);

//Texture2D first : register(t11);
//Texture2D second : register(t12);
//Texture2D third : register(t13);
//Texture2D fourth : register(t14);


struct PixelOutput
{
    float4 color : SV_Target0;
};


float4 PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    float4 albedo = float4(1, 1, 1, 1);
    
    if (hasDiffuseMap)
        albedo = diffuseMap.Sample(samp, input.uv);
    
    float4 layer1 = Layers[0].Sample(samp, input.uv);
    float4 layer2 = Layers[1].Sample(samp, input.uv);
    float4 layer3 = Layers[2].Sample(samp, input.uv);
    float4 layer4 = Layers[3].Sample(samp, input.uv);
    
    //float4 layer1 = first.Sample(samp, input.uv);
    //float4 layer2 = second.Sample(samp, input.uv);
    //float4 layer3 = third.Sample(samp, input.uv);
    //float4 layer4 = fourth.Sample(samp, input.uv);
    
    
    albedo = lerp(albedo, layer1, input.alpha.r);
    albedo = lerp(albedo, layer2, input.alpha.g);
    albedo = lerp(albedo, layer3, input.alpha.b);
    albedo = lerp(albedo, layer4, input.alpha.a);
    
    float3 light = normalize(lights[0].direction); // light 1개만 영향받도록 되어있다. 추후수정 필요할듯?
    
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.binormal);
    float3 N = normalize(input.normal);
    
    float3 normal = N;
    
    if (hasNormalMap)
    {
        float4 normalMapping = normalMap.Sample(samp, input.uv);
    
        float3x3 TBN = float3x3(T, B, N);
        normal = normalMapping * 2.0f - 1.0f;
        normal = normalize(mul(normal, TBN));
    }
    
    float3 viewDir = normalize(input.viewDir);
    
    float diffuseIntensity = saturate(dot(normal, -light));
    
    float4 specular = 0;
    
    if (diffuseIntensity > 0)
    {
        float3 halfWay = normalize(viewDir + light);
        specular = saturate(dot(-halfWay, normal));
        
        float4 specularIntensity = float4(1, 1, 1, 1);
        if (hasSpecularMap)
            specularIntensity = specularMap.Sample(samp, input.uv);
        
        specular = pow(specular, shininess) * specularIntensity;
    }
    
    float4 diffuse = albedo * diffuseIntensity * mDiffuse;
    specular *= mSpecular;
    float4 ambient = albedo * mAmbient;
    
    float4 brushColor = float4(BrushColor(input.worldPos), 1.0f);
    
    //output.color = diffuse + specular + ambient + brushColor;
    
    return diffuse + specular + ambient + brushColor;
}