
Texture2D input : register(t0);

struct OutputDesc
{
    float ab;
};
RWStructuredBuffer<OutputDesc> output; // UAV.
//RWTexture2DArray<float4> Output;

[numthreads(32, 32, 1)]
void CS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint index = groupID.x * 32 * 32 + groupIndex;
    //float4 color = Input.Load(int4(index, 0));
    
    ////Output[index] = float4(1.0 - color.rgb, 1);
    //Output[index] = float4(1.0f, 0.0f, 0.0f, 1.0f);
    ////Output[index] = float4(color.rgb, 1);
    
    

}