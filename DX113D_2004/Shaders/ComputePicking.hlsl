cbuffer Ray : register(b0) // 28byte
{
    float3 position; // 카메라위치(원점)
    float outputSize; // 폴리곤개수
    
    float3 direction; // x,y,z
}

struct InputDesc // 폴리곤단위.
{
    uint index;
    float3 v0;
    float3 v1;
    float3 v2;
};
StructuredBuffer<InputDesc> input; // 폴리곤배열.

struct OutputDesc
{
    int picked;
    float u;
    float v;
    float distance;
};
RWStructuredBuffer<OutputDesc> output; // UAV.


void Intersection(uint index)
{
    float3 A = input[index].v0; // 폴리곤 각 버텍스.
    float3 B = input[index].v1;
    float3 C = input[index].v2;
    
    float3 e1 = B - A; // A->B 벡터
    float3 e2 = C - A; // A->C 벡터.
    
    float3 P, T, Q;
    P = cross(direction, e2);
    
    float d = 1.0f / dot(e1, P);
    
    T = position - A;
    output[index].u = dot(T, P) * d;
    
    Q = cross(T, e1);
    output[index].v = dot(direction, Q) * d;
    output[index].distance = dot(e2, Q) * d;
    
    bool b = (output[index].u >= 0.0f) &&
                (output[index].v >= 0.0f) &&
                (output[index].u + output[index].v <= 1.0f) &&
                (output[index].distance >= 0.0f);
    
    
    
    output[index].picked = b ? 1 : 0;
}

[numthreads(32, 32, 1)] // 하나의 스레드그룹에서 실행할 스레드의 개수.
void CS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
//void CS(uint3 index : SV_DispatchThreadID)
{
    uint index = groupID.x * 32 * 32 + groupIndex; // 정확힌 모르겠지만 이렇게하면 인덱스에 0부터 대입해서 쭉~ +1씩
    // 
    
    if(outputSize > index) // 폴리곤개수가 인덱스보다 더 크면? => 폴리곤개수만큼만 계산.
        Intersection(index);
}