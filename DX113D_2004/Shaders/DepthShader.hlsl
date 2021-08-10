#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position; // PixelShader���� NDC��ǥ�� �ٲ�.
    float4 sPos : POSITION1; // PixelShader���� ����� ������ Ŭ���ΰ�����ǥ.
    float2 uv : UV;
   
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.sPos = output.pos;
    
    output.uv = input.uv;
    
    return output;
}

struct PixelOutput
{
    //float4 color : SV_Target0; // ������ �� �ε����� rtv�� �Ѵٴ°�.
    float4 color;
};



PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    // �׸��ڰ����.
    float depth = input.sPos.z / input.sPos.w;
    depth = depth * 20.0f;
    output.color = float4(depth, depth, depth, 1.0f);
    
    return output;
}