#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 viewDir : ViewDir;
};

PixelInput VS(VertexUVNormal input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    
    float3 camPos = invView._41_42_43; // 카메라의 월드위치.
    output.viewDir = normalize(output.pos.xyz - camPos); // world를 곱한 이후에 계산해야함.
                                                         // world 곱하기 이전에 계산하면 안됨. 그건 지역좌표임
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);    
    
    output.normal = normalize(mul(input.normal, (float3x3) world)); // 월드상의 법선좌표저장(정규화시켜서)   
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float4 albedo = diffuseMap.Sample(samp, input.uv);
    
    float3 light = normalize(lightDirection);
    float3 normal = normalize(input.normal); // 이미 정규화된거긴한데 혹시모르니 안전하게 한번더.
    float3 viewDir = normalize(input.viewDir);
    
    float diffuseIntensity = saturate(dot(normal, -light)); // 디퓨즈값.
    
    float4 specular = 0;

    if (diffuseIntensity > 0) // 디퓨즈값(난반사)가 있으면 스펙큘러값(정반사) 계산. 난반사가 있어야 정반사도있다.
    {
        float3 halfWay = normalize(viewDir + light);
        specular = saturate(dot(-halfWay, normal));
        
        float4 specularIntensity = float4(1, 1, 1, 1);
        if (hasSpecularMap)
            specularIntensity = specularMap.Sample(samp, input.uv);
        
        specular = pow(specular, shininess) * specularIntensity;
        // shininess값만큼 제곱해줌으로써 정반사가 좀 더 정교해짐. cos값의 그래프가 좀 더 경사져지기 때문.
    }
    
    float4 diffuse = albedo * diffuseIntensity * mDiffuse; // 텍스쳐맵의 픽셀색 * 디퓨즈값 * mDiffuse
    specular *= mSpecular;
    float4 ambient = albedo * mAmbient;
    
    return diffuse + specular + ambient;
}