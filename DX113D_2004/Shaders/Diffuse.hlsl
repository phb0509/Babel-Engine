cbuffer World : register(b0)
{
    matrix world;
}

cbuffer View : register(b1)
{
    matrix view;
}

cbuffer Projection : register(b2)
{
    matrix projection;
}

cbuffer Light : register(b3)
{
    float3 lightDirection;
}

struct VertexInput
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
};

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : UV;
    float diffuse : Diffuse;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    float3 light = normalize(lightDirection);
    float3 normal = normalize(mul(input.normal, (float3x3)world));
    
    output.diffuse = saturate(dot(normal, -light)); // saturate함수는 0~1의 범위로 값을 바꿔준다.
                                                    // 이 함수를 써야하는 이유는, 내적했을 때 cos90도를 넘어가면 값이 -로 넘어가게되는데
                                                    // 그러면 아래 픽셀셰이더에서 -값을 곱해버리기때문에 ambient(간접광)을 더해줘도
                                                    // 그냥 새까맣게나온다. 그래서 saturate함수로 0~1사이로 바꿔줘야한다.
    
    output.uv = input.uv;
    
    return output;
}

Texture2D map : register(t0);
SamplerState samp : register(s0);

struct PixelOutput
{
    float4 color : SV_Target0;
};


PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    float4 color = map.Sample(samp, input.uv);
    float4 ambient = color * 0.1f;
    output.color = color * input.diffuse + ambient;
   // output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
    //return color * input.diffuse + ambient; // difuse값이 1이면 색깔 그대로. 낮아질수록 어두워질것.
}