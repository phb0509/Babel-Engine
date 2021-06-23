#include "Header.hlsli"

//VS///////////////////////////////////////////
struct VertexOutput
{
    float4 pos : Position;
    float2 uv : UV;
};

VertexOutput VS(VertexOutput input)
{
    VertexOutput output;
    output.pos = input.pos;
    output.uv = input.uv;
    
    return output;
}

//HS/////////////////////////////////////////////////
struct CHullOutput
{
    float edge[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

cbuffer TerrainBuffer : register(b10)
{
    float2 dist;
    float2 tessFactor; // 이 2개의 값을 이용해서 얼마나 쪼갤지.
    
    float cellSpacing; // 셀사이의 공간
    float cellSpacingU; // 셀 각각의 uv값
    float cellSpacingV;
    float heightScale; // 높이보간할때, 높이정보값으로 사용.
    
    float4 culling[6];
}

float CalcTessFactor(float3 position) // 카메라의 거리에 따라 얼마나 쪼갤지 계산.
{
    float d = distance(position, invView._41_42_43); // 매개변수position과 카메라의 거리.
    float f = saturate((d - dist.y) / (dist.x - dist.y)); // factor. LOD 구현 범위. 카메라와의 거리의 비율.
    
    return lerp(tessFactor.x, tessFactor.y, f); // X랑 Y사이에서 F값으로 보간해서 반환.
}

bool OutFrustumPlane(float3 center, float3 extent, float4 plane) // center와 extent를 합쳐서 하나의 큐브로 생각. 그 큐브가 plane에 들어오는지 검사.
{
    float3 n = abs(plane.xyz);
    float r = dot(extent, n);
    float s = dot(float4(center, 1), plane);
    
    return (s + r) < 0.0f;
}

bool OutFrustum(float3 center, float3 extent)
{
    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        [flatten]
        if (OutFrustumPlane(center, extent, culling[i]))
            return true;
    }
    
    return false;
}

#define NUM_CONTROL_POINTS 4

CHullOutput
    CHS(
    InputPatch<VertexOutput, NUM_CONTROL_POINTS> input)   // Constant HullShader 제일 먼저 진입하는곳.
{
    float4 position[4];
    position[0] = mul(input[0].pos, world); // 각 컨트롤 포인트들 월드값으로 변환.
    position[1] = mul(input[1].pos, world);
    position[2] = mul(input[2].pos, world);
    position[3] = mul(input[3].pos, world);
    
    float minY = 0.0f;
    float maxY = heightScale;
    
    float3 minBox = float3(position[2].x, minY, position[2].z);
    float3 maxBox = float3(position[1].x, maxY, position[1].z);
    
    float3 boxCenter = (minBox + maxBox) * 0.5f;
    float3 boxExtent = abs(maxBox - minBox) * 0.5f;
    
    CHullOutput output;
    
    [flatten]
    if (OutFrustum(boxCenter, boxExtent)) // 프러스텀밖이라면
    {
        output.edge[0] = -1; // 분할을 하지않겠다.
        output.edge[1] = -1;
        output.edge[2] = -1;
        output.edge[3] = -1;
    
        output.inside[0] = -1;
        output.inside[1] = -1;
        
        return output;
    }
    
    float3 e0 = (position[0] + position[2]).xyz * 0.5f; // 각 꼭지점 두개의 사이 점.
    float3 e1 = (position[0] + position[1]).xyz * 0.5f;
    float3 e2 = (position[1] + position[3]).xyz * 0.5f;
    float3 e3 = (position[2] + position[3]).xyz * 0.5f;
    
    float3 center = (e0 + e2) * 0.5f; // 사각형의 중앙점
    
    output.edge[0] = CalcTessFactor(e0);
    output.edge[1] = CalcTessFactor(e1);
    output.edge[2] = CalcTessFactor(e2);
    output.edge[3] = CalcTessFactor(e3);
    
    output.inside[0] = CalcTessFactor(center);
    output.inside[1] = CalcTessFactor(center);
    
    return output;
}

struct HullOutput
{
    float4 pos : Position;
    float2 uv : UV;
};

[domain("quad")]
//[partitioning("integer")]
[partitioning("fractional_even")] // 조금 더 부드러워진다...
[outputtopology("triangle_cw")] // cw = clock wise // 시계방향, 다른옵션으로는 ccw // count clock wise // 반시계방향.
[outputcontrolpoints(4)]
[patchconstantfunc("CHS")]
HullOutput HS(InputPatch<VertexOutput, NUM_CONTROL_POINTS> input,
uint i : SV_OutputControlPointID) // 그냥 HullShader. 값 그대로 DomainShader로 넘긴다.
{
    HullOutput output;
    output.pos = input[i].pos;
    output.uv = input[i].uv;
    
    return output;
}

struct DomainOutput
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

Texture2D heightMap : register(t0);

[domain("quad")]
DomainOutput DS(CHullOutput input, float2 uv : SV_DomainLocation,
const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch) // DomainShader.쪼개진 버텍스들에 대해 각각 수행.
{
    DomainOutput output;
    
    float4 v1 = lerp(patch[0].pos, patch[1].pos, uv.x); 
    float4 v2 = lerp(patch[2].pos, patch[3].pos, uv.x);
    float4 position = lerp(v1, v2, uv.y); // 새 정점의 위치.
    
    float2 uv1 = lerp(patch[0].uv, patch[1].uv, uv.x);
    float2 uv2 = lerp(patch[2].uv, patch[3].uv, uv.x);
    float2 texCoord = lerp(uv1, uv2, uv.y);
    
    position.y = heightMap.SampleLevel(samp, texCoord, 0).r * heightScale;
    
    output.pos = float4(position.xyz, 1.0f);
    output.pos = mul(output.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.uv = texCoord;
    
    return output;
}

float4 PS(DomainOutput input) : SV_Target
{
    return diffuseMap.Sample(samp, input.uv);
}