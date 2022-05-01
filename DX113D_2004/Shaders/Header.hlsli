#define MAX_BONE 256
#define MAX_LIGHT 10
#define MAX_INSTANCE 600

//VertexShaderBuffer
cbuffer World : register(b0)
{
    matrix world;    
}

cbuffer View : register(b1)
{
    matrix view;
    matrix invView;
}

cbuffer Projection : register(b2)
{
    matrix projection;
    matrix invProjection;
}

cbuffer Bone : register(b3)
{
    matrix bones[MAX_BONE];        
}

struct KeyFrame
{
    int clip;
    uint curFrame;
    uint nextFrame;
    float time;
    
    float runninTime;    
    float speed;
    float2 padding;
};

struct TweenFrame
{
    float takeTime;
    float tweenTime;
    float runningTime;
    float padding;
    
    KeyFrame cur; // 32byte
    KeyFrame next; // 32byte
};

cbuffer Frame : register(b4)
{
    TweenFrame tweenFrame[MAX_INSTANCE]; // 개당 80byte.
}

cbuffer ModelType : register(b5)
{
    int modelType;
}

Texture2DArray transformMap : register(t0);

//PixelShaderBuffer
static const float2 specPowerRange = { 0.1f, 50.f };

struct Light // 64byte
{
    float4 color;
    
    float3 direction;
    int type;

    float3 position;
    float range;
    
    float inner;
    float outer;
    
    float length;
    int active;
};

cbuffer Light : register(b0) // 688 byte
{
    Light lights[MAX_LIGHT]; // 64 * 10 = 640byte
    uint lightCount;
    
    float3 padding;
    
    float4 ambient;
    float4 ambientCeil;
}

struct Material
{
    float3 normal;
    float4 diffuseColor;
    float4 emissive;
    float4 specularIntensity;
    float shininess;
    float3 camPos;    
    float3 worldPos;    
};

cbuffer MaterialInfo : register(b1) // 80byte
{
    float4 mDiffuse;        
    float4 mSpecular;        
    float4 mAmbient;
    float4 mEmissive;
    
    float shininess;
    
    int hasDiffuseMap;
    int hasSpecularMap;
    int hasNormalMap;
}

SamplerState samp : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D normalMap : register(t2);
Texture2D brushMap : register(t7);

//Texture2D<float> depthTexture : register(t3);
//Texture2D depthTexture : register(t3);
//Texture2D diffuseTexture : register(t4);
//Texture2D specularTexture : register(t5);
//Texture2D normalTexture : register(t6);

//VertexLayouts
struct Vertex
{
    float4 pos : Position;    
};

//struct VertexColor 
//{
//    float4 pos : Position;
//    float4 color : Color;
//    float4 colorForPicking : ColorForPicking;
//};

struct VertexColor
{
    float4 pos : Position;
    float4 colorForPicking : ColorForPicking;
};

struct VertexUV
{
    float4 pos : Position;
    float2 uv : UV;    
};

struct VertexUVNormal
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
};

struct VertexUVNormalTangent
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
};

struct VertexUVNormalTangentAlpha
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float4 alpha : Alpha;
};

struct VertexUVNormalTangentBlend
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float4 indices : BlendIndices;
    float4 weights : BlendWeights;
};

struct VertexInstancing
{
    float4 pos : Position;
    float2 uv : UV;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float4 indices : BlendIndices;
    float4 weights : BlendWeights;
    
    matrix transform : Instance_Transform;
    uint index : Instance_Index;
    uint instanceID : SV_InstanceID;
};

//VertexShader////////////////////////////////////////////////////////////////
matrix BoneWorld(float4 indices, float4 weights)
{
    matrix transform = 0;
    
    transform += mul(weights.x, bones[(uint) indices.x]);
    transform += mul(weights.y, bones[(uint) indices.y]);
    transform += mul(weights.z, bones[(uint) indices.z]);
    transform += mul(weights.w, bones[(uint) indices.w]);
    
    return transform;
}

matrix SkinWorld(float4 indices, float4 weights)
{
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix transform = 0;
    matrix cur = 0, next = 0;
    matrix curAnim = 0, nextAnim = 0;
    
    int clip[2];
    int curFrame[2];
    int nextFrame[2];
    float time[2];
    
    clip[0] = tweenFrame[0].cur.clip;
    curFrame[0] = tweenFrame[0].cur.curFrame;
    nextFrame[0] = tweenFrame[0].cur.nextFrame;
    time[0] = tweenFrame[0].cur.time;
    
    clip[1] = tweenFrame[0].next.clip;
    curFrame[1] = tweenFrame[0].next.curFrame;
    nextFrame[1] = tweenFrame[0].next.nextFrame;
    time[1] = tweenFrame[0].next.time;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        /*
           Sample을 안쓰고 Load 사용.
           Sample은 레스터라이징에서 확대,축소가 일어나기 때문에 원본값을 가져오기 위함.
           indices : 본 번호, 4픽셀당 하나의 행렬이므로 4를 곱해주고 0,1,2,3을 더해줌으로써 1,2,3,4 번째 픽셀을 가리킴
           currFrame : 현재 프레임의 번호
           clip : 면
           w는 밉맵인데 사용하지 않기 때문에 .
        */
        c0 = transformMap.Load(int4(indices[i] * 4 + 0, curFrame[0], clip[0], 0)); // 첫번째 픽셀
        c1 = transformMap.Load(int4(indices[i] * 4 + 1, curFrame[0], clip[0], 0)); // 두번째 픽셀
        c2 = transformMap.Load(int4(indices[i] * 4 + 2, curFrame[0], clip[0], 0)); // 세번째 픽셀
        c3 = transformMap.Load(int4(indices[i] * 4 + 3, curFrame[0], clip[0], 0)); // 네번째 픽셀
        cur = matrix(c0, c1, c2, c3); // 가중치에 영향을 받을 동작
        
        // 다음 동작
        n0 = transformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = transformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = transformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = transformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3); // 가중치에 영향을 받을 다음 동작.
        
        // 현재 동작에서 다음동작으로 lerp 해서 동작을 부드럽게 이어준다.
        curAnim = lerp(cur, next, time[0]);
        
        [flatten]
        // 다음 클립
        if(clip[1] > -1)
        {
            c0 = transformMap.Load(int4(indices[i] * 4 + 0, curFrame[1], clip[1], 0));
            c1 = transformMap.Load(int4(indices[i] * 4 + 1, curFrame[1], clip[1], 0));
            c2 = transformMap.Load(int4(indices[i] * 4 + 2, curFrame[1], clip[1], 0));
            c3 = transformMap.Load(int4(indices[i] * 4 + 3, curFrame[1], clip[1], 0));
            cur = matrix(c0, c1, c2, c3);
        
            n0 = transformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = transformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = transformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = transformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(cur, next, time[1]);
            
            curAnim = lerp(curAnim, nextAnim, tweenFrame[0].tweenTime);
        }
        
        transform += mul(weights[i], curAnim); // 가중치를 곱해서 더해줌.
    }

    return transform;
}

matrix SkinWorld(int instanceID, float4 indices, float4 weights)
{
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix transform = 0;
    matrix cur = 0, next = 0;
    matrix curAnim = 0, nextAnim = 0;
    
    int clip[2];
    int curFrame[2];
    int nextFrame[2];
    float time[2];
    
    clip[0] = tweenFrame[instanceID].cur.clip;
    curFrame[0] = tweenFrame[instanceID].cur.curFrame;
    nextFrame[0] = tweenFrame[instanceID].cur.nextFrame;
    time[0] = tweenFrame[instanceID].cur.time;
    
    clip[1] = tweenFrame[instanceID].next.clip;
    curFrame[1] = tweenFrame[instanceID].next.curFrame;
    nextFrame[1] = tweenFrame[instanceID].next.nextFrame;
    time[1] = tweenFrame[instanceID].next.time;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = transformMap.Load(int4(indices[i] * 4 + 0, curFrame[0], clip[0], 0));
        c1 = transformMap.Load(int4(indices[i] * 4 + 1, curFrame[0], clip[0], 0));
        c2 = transformMap.Load(int4(indices[i] * 4 + 2, curFrame[0], clip[0], 0));
        c3 = transformMap.Load(int4(indices[i] * 4 + 3, curFrame[0], clip[0], 0));
        cur = matrix(c0, c1, c2, c3);
        
        n0 = transformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = transformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = transformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = transformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        curAnim = lerp(cur, next, time[0]);
        
        [flatten]
        if (clip[1] > -1)
        {
            c0 = transformMap.Load(int4(indices[i] * 4 + 0, curFrame[1], clip[1], 0));
            c1 = transformMap.Load(int4(indices[i] * 4 + 1, curFrame[1], clip[1], 0));
            c2 = transformMap.Load(int4(indices[i] * 4 + 2, curFrame[1], clip[1], 0));
            c3 = transformMap.Load(int4(indices[i] * 4 + 3, curFrame[1], clip[1], 0));
            cur = matrix(c0, c1, c2, c3);
        
            n0 = transformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = transformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = transformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = transformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(cur, next, time[1]);
            
            curAnim = lerp(curAnim, nextAnim, tweenFrame[instanceID].tweenTime);
        }
        
        transform += mul(weights[i], curAnim);
    }

    return transform;
}

//PixelShader//////////////////////////////////////////////
float3 NormalMapping(float3 T, float3 B, float3 N, float2 uv)
{
    T = normalize(T);
    B = normalize(B);
    N = normalize(N);
    
    if(hasNormalMap)
    {
        float3 normal = normalMap.Sample(samp, uv).rgb;
                
        float3x3 TBN = float3x3(T, B, N);
        N = normal * 2.0f - 1.0f;
        N = normalize(mul(N, TBN));
    }
    
    return N;
}

float4 SpecularMapping(float2 uv)
{
    [flatten]
    if(hasSpecularMap)
        return specularMap.Sample(samp, uv);
    
    return float4(1, 1, 1, 1);
}

float4 CalcAmbient(Material material)
{
    float up = material.normal.y * 0.5f + 0.5f; // -1 ~ 1 -> 1 ~ 1 정규화.
    
    float3 resultAmbient = ambient + up * ambientCeil; // 라이트버퍼에 있는 값들.
    
    return float4(resultAmbient, 1.0f) * material.diffuseColor;
}

float4 CalcDirectional(Material material, Light light)
{
    float3 toLight = -normalize(light.direction);
    
    float NDotL = saturate(dot(toLight, material.normal));
    float4 finalColor = light.color * NDotL * mDiffuse;
    
    [flatten]
    if (NDotL > 0)
    {
        float3 toEye = normalize(material.camPos - material.worldPos);
        float3 halfWay = normalize(toEye + toLight);
        float NDotH = saturate(dot(material.normal, halfWay));
        
        finalColor += light.color * pow(NDotH, shininess) * material.specularIntensity;
    }
    
    return finalColor * material.diffuseColor;
}

float4 CalcPoint(Material material, Light light)
{
    float3 toLight = light.position - material.worldPos;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;
    
    float NDotL = saturate(dot(toLight, material.normal));
    float4 finalColor = light.color * NDotL * mDiffuse;
    
    [flatten]
    if (NDotL > 0)
    {
        float3 toEye = normalize(material.camPos - material.worldPos);
        float3 halfWay = normalize(toEye + toLight);
        float NDotH = saturate(dot(material.normal, halfWay));
        
        finalColor += light.color * pow(NDotH, shininess) * material.specularIntensity;
    }
    
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / light.range);
    float attention = distanceToLightNormal * distanceToLightNormal;
    
    return finalColor * material.diffuseColor * attention;
}

float4 CalcSpot(Material material, Light light)
{
    float3 toLight = light.position - material.worldPos;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;
    
    float NDotL = saturate(dot(toLight, material.normal));
    float4 finalColor = light.color * NDotL * mDiffuse;
    
    [flatten]
    if (NDotL > 0)
    {
        float3 toEye = normalize(material.camPos - material.worldPos);
        float3 halfWay = normalize(toEye + toLight);
        float NDotH = saturate(dot(material.normal, halfWay));
        
        finalColor += light.color * pow(NDotH, shininess) * material.specularIntensity;
    }
    
    float3 dir = -normalize(light.direction);
    float cosAngle = dot(dir, toLight);
    
    float outer = cos(radians(light.outer));
    float inner = 1.0f / cos(radians(light.inner));
    
    float conAttention = saturate((cosAngle - outer) * inner);
    conAttention *= conAttention;
    
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / light.range);
    float attention = distanceToLightNormal * distanceToLightNormal;
    
    return finalColor * material.diffuseColor * attention * conAttention;
}

float4 CalcCapsule(Material material, Light light)
{
    float3 direction = normalize(light.direction);
    float3 start = material.worldPos - light.position;
    float distanceOnLine = dot(start, direction) / light.length;
    distanceOnLine = saturate(distanceOnLine) * light.length;
    
    float3 pointOnLine = light.position + direction * distanceOnLine;
    
    float3 toLight = pointOnLine - material.worldPos;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;
    
    float NDotL = saturate(dot(toLight, material.normal));
    float4 finalColor = light.color * NDotL * mDiffuse;
    
    [flatten]
    if (NDotL > 0)
    {
        float3 toEye = normalize(material.camPos - material.worldPos);
        float3 halfWay = normalize(toEye + toLight);
        float NDotH = saturate(dot(material.normal, halfWay));
        
        finalColor += light.color * pow(NDotH, shininess) * material.specularIntensity;
    }
    
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / light.range);
    float attention = distanceToLightNormal * distanceToLightNormal;
    
    return finalColor * material.diffuseColor * attention;
}

float4 CalcLights(Material material)
{
    float4 result = 0;
    
    for (int i = 0; i < lightCount; i++)
    {
        [flatten]
        if (!lights[i].active)
        {
            continue;
        }
           
        [flatten]
        if (lights[i].type == 0)
            result += CalcDirectional(material, lights[i]);
        else if (lights[i].type == 1)
            result += CalcPoint(material, lights[i]);
        else if (lights[i].type == 2)
            result += CalcSpot(material, lights[i]);
        else if (lights[i].type == 3)
            result += CalcCapsule(material, lights[i]);
    }
    
    return result;
}

float4 CalcEmissive(Material material)
{
    float emissive = 0.0f;
    
    [flatten]
    if (material.emissive.a > 0.0f)
    {
        float3 E = normalize(material.camPos - material.worldPos);
        
        float NdotE = dot(material.normal, E);
        emissive = smoothstep(1.0f - mEmissive.a, 1.0f, 1.0f - saturate(NdotE));
    }
    
    return float4(material.emissive.rgb * emissive, 1.0f);
}