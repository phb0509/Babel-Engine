#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position; // PixelShader���� NDC��ǥ�� �ٲ�.
    float4 sPos : POSITION1; // PixelShader���� ����� ������ Ŭ���ΰ�����ǥ.
};

PixelInput VS(VertexUVNormalTangentBlend input)
{
    PixelInput output;
    
    input.pos.w = 1.0f;
    
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.sPos = output.pos;
    
    return output;
}

struct PixelOutput
{
    float4 color : SV_Target0; // ������ �� �ε����� rtv�� �Ѵٴ°�.
};

PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    // �׸��ڰ����.
    float depthValue = input.pos.z / input.pos.w;
    
    depthValue = depthValue * 80.0f;
    //depthValue = depthValue;
    output.color = float4(depthValue, depthValue, depthValue, 1.0f);
    //output.color = float4(1.0f,1.0f,0.0f,1.0f);
    // all 1.0f�� ���.
    
    return output;

}