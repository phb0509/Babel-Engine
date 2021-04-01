#pragma once

class Effect
{
public:
	struct VertexParticle
	{
		Float3 position = Float3(0, 0, 0);
		Float2 uv = Float2(0, 0);//(-1 ~ 1)
		Float3 velocity = Float3(0, 0, 0);
		Float4 random = Float4(0, 0, 0, 0);
		float time = 0.0f;;
	};

private:
	EffectBuffer* buffer;
	EffectData data;

	Material* material;
	Mesh* mesh;

	VertexParticle* vertices;
	UINT* indices;

	float curTime;
	float lastAddTime;
	
	UINT leadCount;
	UINT gpuCount;
	UINT activeCount;
	UINT deactiveCount;

	BlendState* blendState[2];
	DepthStencilState* depthState[2];
public:
	Effect(string file);
	~Effect();

	void Reset();
	void Add(Vector3& position);

	void Update();
	void Render();

	void ReadFile(string file);

	EffectData& GetData() { return data; }
	void SetTexture(wstring file);
private:
	void MapVertices();
	void Activation();
	void Deactivation();	
};