#include "Header.hlsli"

struct VertexOutput
{
    float4 pos : Position;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
    output.pos = input.pos;
    
	return output;
}

struct CHullOutput // 얼마나 쪼갤지.
{
    float edge[3] : SV_TessFactor; // 간선(선분)을 몇개로 쪼갤지.
    float inside : SV_InsideTessFactor; // 중점
};

cbuffer EdgeInfo : register(b10)
{
    int edge0;
    int edge1;
    int edge2;
    int inside;
}

#define NUM_CONTROL_POINTS 3

CHullOutput CHS(InputPatch<VertexOutput, NUM_CONTROL_POINTS> input) // 얼마나 쪼갤건가
{
    CHullOutput output;
    
    output.edge[0] = edge0; // 우항의 값만큼 쪼갠다.
    output.edge[1] = edge1;
    output.edge[2] = edge2;
    output.inside = inside; // 중점을 몇개를 두고 쪼갤것인가?
    
    return output;
}

struct HullOutput // 어떻게 쪼갤건가 (도메인 셰이더로 넘길거임)
{
    float4 pos : Position;
};

[domain("tri")] // 어떻게 도메인으로 넘길건지. 트라이앵글로 넘길것이다.
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CHS")] // 위에 함수이름. CHullOutput를 리턴하는 함수의 이름
HullOutput HS(InputPatch<VertexOutput, NUM_CONTROL_POINTS> input,
uint i : SV_OutputControlPointID) // SV 붙었으니까 시스템에서 받는 값.
{
    HullOutput output;
    output.pos = input[i].pos;
    
    return output;
} // CS에선 딱히 별작업안하고 넘어간다.

struct DomainOutput
{
    float4 pos : SV_Position; // 도메인셰이더에서 바로 RS로 넘기니까 SV를 붙여야함. (시스템으로 넘기니까)
};

[domain("tri")] // 위의 tri와 반드시 같은게 아니다. 삼각형으로 넘겨받아서 사각형으로 만들수도 있긴한데 매우 복잡해짐. 일반적으론 같이 맞춰준다.
DomainOutput DS(CHullOutput input, float3 uvw : SV_DomainLocation, // 여기서 SV는 RS가 아니라 TS. TS에서 쪼갠 다음 정보를 준다.
const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
    DomainOutput output;
    
    float4 position = patch[0].pos * uvw.x + patch[1].pos * uvw.y + patch[2].pos * uvw.z;
    output.pos = float4(position.xyz, 1.0f);
    
    return output;
}

float4 PS(DomainOutput input) : SV_Target
{
    return float4(0, 1, 0, 1);
}