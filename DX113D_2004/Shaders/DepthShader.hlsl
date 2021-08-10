#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position; // PixelShader에서 NDC좌표로 바뀜.
    float4 sPos : POSITION1; // PixelShader에서 사용할 정점의 클리핑공간좌표.
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
    //float4 color : SV_Target0; // 리턴을 이 인덱스의 rtv에 한다는것.
    float4 color;
};



PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    // 그림자값계산.
    float depth = input.sPos.z / input.sPos.w;
    depth = depth * 20.0f;
    output.color = float4(depth, depth, depth, 1.0f);
    
    return output;
}