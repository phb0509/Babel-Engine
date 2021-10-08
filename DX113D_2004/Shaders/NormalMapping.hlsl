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
    
//    float3 camPos = invView._41_42_43; // 카메라의 world좌표.
//    output.viewDir = normalize(output.pos.xyz - camPos); // 카메라->버텍스 방향벡터.
    
//    output.pos = mul(output.pos, view);
//    output.pos = mul(output.pos, projection);
    
//    output.normal = mul(input.normal, (float3x3) world);
//    output.tangent = mul(input.tangent, (float3x3) world);
//    output.binormal = cross(output.normal, output.tangent); // 외적해서 나머지 축 구해주기.
    
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
    
//    if (hasNormalMap) // 노말맵 있으면
//    {
//        float4 normalMapping = normalMap.Sample(samp, input.uv); // 노말맵 RGB값 가져오기.
    
//        float3x3 TBN = float3x3(T, B, N); // 월드공간 -> 접선공간으로 변경. 변환행렬
//        normal = normalMapping * 2.0f - 1.0f; // 노말맵의 RGB값(0~1)을 벡터단위(-1~1)로 변환함.
//        normal = normalize(mul(normal, TBN)); // return 1*3 행렬
//    }
    
//    float3 viewDir = normalize(input.viewDir);
    
//    float diffuseIntensity = saturate(dot(normal, -light)); // 디퓨즈량 값 계산(0~1로 변환)
    
//    float4 specular = 0;

//    if (diffuseIntensity > 0) // 디퓨즈값이 있으면, 정반사 계산.
//    {
//        float3 halfWay = normalize(viewDir + light);
//        specular = saturate(dot(-halfWay, normal));
        
//        float4 specularIntensity = float4(1, 1, 1, 1);
//        if (hasSpecularMap)
//            specularIntensity = specularMap.Sample(samp, input.uv);
        
//        specular = pow(specular, shininess) * specularIntensity;
//    }
    
//    float4 diffuse = albedo * diffuseIntensity * mDiffuse; // 텍스쳐 픽셀 * 디퓨즈량 *
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