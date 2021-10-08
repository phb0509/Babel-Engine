//#include "Header.hlsli"

//struct PixelInput
//{
//    float4 pos : SV_Position;
//    float2 uv : UV;
//    float3 normal : Normal;
//    float3 tangent : Tangent;
//    float3 binormal : Binormal;
//    float3 viewDir : ViewDir;
//};

//PixelInput VS(VertexUVNormalTangent input)
//{
//    PixelInput output;
    
//    output.pos = mul(input.pos, world);
    
//    float3 camPos = invView._41_42_43; // ī�޶��� world��ǥ.
//    output.viewDir = normalize(output.pos.xyz - camPos); // ī�޶�->���ؽ� ���⺤��.
    
//    output.pos = mul(output.pos, view);
//    output.pos = mul(output.pos, projection);
    
//    output.normal = mul(input.normal, (float3x3) world);
//    output.tangent = mul(input.tangent, (float3x3) world);
//    output.binormal = cross(output.normal, output.tangent); // �����ؼ� ������ �� �����ֱ�.
    
//    output.uv = input.uv;
    
//    return output;
//}

//float4 PS(PixelInput input) : SV_Target
//{
//    float4 albedo = float4(1, 1, 1, 1);

//    if (hasDiffuseMap) 
//        albedo = diffuseMap.Sample(samp, input.uv);
    
//    float3 light = normalize(lightDirection);
    
//    float3 T = normalize(input.tangent);
//    float3 B = normalize(input.binormal);
//    float3 N = normalize(input.normal);
    
//    float3 normal = N;
    
//    if (hasNormalMap) // �븻�� ������
//    {
//        float4 normalMapping = normalMap.Sample(samp, input.uv); // �븻�� RGB�� ��������.
    
//        float3x3 TBN = float3x3(T, B, N); // ������� -> ������������ ����. ��ȯ���
//        normal = normalMapping * 2.0f - 1.0f; // �븻���� RGB��(0~1)�� ���ʹ���(-1~1)�� ��ȯ��.
//        normal = normalize(mul(normal, TBN)); // return 1*3 ���
//    }
    
//    float3 viewDir = normalize(input.viewDir);
    
//    float diffuseIntensity = saturate(dot(normal, -light)); // ��ǻ� �� ���(0~1�� ��ȯ)
    
//    float4 specular = 0;

//    if (diffuseIntensity > 0) // ��ǻ��� ������, ���ݻ� ���.
//    {
//        float3 halfWay = normalize(viewDir + light);
//        specular = saturate(dot(-halfWay, normal));
        
//        float4 specularIntensity = float4(1, 1, 1, 1);
//        if (hasSpecularMap)
//            specularIntensity = specularMap.Sample(samp, input.uv);
        
//        specular = pow(specular, shininess) * specularIntensity;
//    }
    
//    float4 diffuse = albedo * diffuseIntensity * mDiffuse; // �ؽ��� �ȼ� * ��ǻ� *
//    specular *= mSpecular;
//    float4 ambient = albedo * mAmbient;
    
//    float emissive = 0.0f;
    
//    [flatten]
//    if (mEmissive.a > 0.0f)
//    {
//        float3 E = -viewDir;
        
//        float NdotE = dot(normal, E);
//        emissive = smoothstep(1.0f - mEmissive.a, 1.0f, 1.0f - saturate(NdotE));
//    }
    
//    return diffuse + specular + ambient + mEmissive * emissive;
//}