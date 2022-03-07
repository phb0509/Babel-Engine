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
    
    output.diffuse = saturate(dot(normal, -light)); // saturate�Լ��� 0~1�� ������ ���� �ٲ��ش�.
                                                    // �� �Լ��� ����ϴ� ������, �������� �� cos90���� �Ѿ�� ���� -�� �Ѿ�ԵǴµ�
                                                    // �׷��� �Ʒ� �ȼ����̴����� -���� ���ع����⶧���� ambient(������)�� �����൵
                                                    // �׳� ����İԳ��´�. �׷��� saturate�Լ��� 0~1���̷� �ٲ�����Ѵ�.
    
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
    //return color * input.diffuse + ambient; // difuse���� 1�̸� ���� �״��. ���������� ��ο�����.
}