#include "Header.hlsli"

struct PixelInput
{
    float4 pos : SV_Position; // PixelShader에서 NDC좌표로 바뀜.
    float4 sPos : POSITION1; // PixelShader에서 사용할 정점의 클리핑공간좌표.
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
    float4 color : SV_Target0; // 리턴을 이 인덱스의 rtv에 한다는것.
};

PixelOutput PS(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    // 그림자값계산.
    float depthValue = input.pos.z / input.pos.w;
    
    depthValue = depthValue * 80.0f;
    //depthValue = depthValue;
    output.color = float4(depthValue, depthValue, depthValue, 1.0f);
    //output.color = float4(1.0f,1.0f,0.0f,1.0f);
    // all 1.0f는 흰색.
    
    return output;

}