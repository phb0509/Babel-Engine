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
    
    float3 camPos = invView._41_42_43; // ī�޶��� ������ġ.
    output.viewDir = normalize(output.pos.xyz - camPos); // world�� ���� ���Ŀ� ����ؾ���.
                                                         // world ���ϱ� ������ ����ϸ� �ȵ�. �װ� ������ǥ��
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);    
    
    output.normal = normalize(mul(input.normal, (float3x3) world)); // ������� ������ǥ����(����ȭ���Ѽ�)   
    
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    float4 albedo = diffuseMap.Sample(samp, input.uv);
    
    float3 light = normalize(lightDirection);
    float3 normal = normalize(input.normal); // �̹� ����ȭ�Ȱű��ѵ� Ȥ�ø𸣴� �����ϰ� �ѹ���.
    float3 viewDir = normalize(input.viewDir);
    
    float diffuseIntensity = saturate(dot(normal, -light)); // ��ǻ�.
    
    float4 specular = 0;

    if (diffuseIntensity > 0) // ��ǻ�(���ݻ�)�� ������ ����ŧ����(���ݻ�) ���. ���ݻ簡 �־�� ���ݻ絵�ִ�.
    {
        float3 halfWay = normalize(viewDir + light);
        specular = saturate(dot(-halfWay, normal));
        
        float4 specularIntensity = float4(1, 1, 1, 1);
        if (hasSpecularMap)
            specularIntensity = specularMap.Sample(samp, input.uv);
        
        specular = pow(specular, shininess) * specularIntensity;
        // shininess����ŭ �����������ν� ���ݻ簡 �� �� ��������. cos���� �׷����� �� �� ��������� ����.
    }
    
    float4 diffuse = albedo * diffuseIntensity * mDiffuse; // �ؽ��ĸ��� �ȼ��� * ��ǻ� * mDiffuse
    specular *= mSpecular;
    float4 ambient = albedo * mAmbient;
    
    return diffuse + specular + ambient;
}