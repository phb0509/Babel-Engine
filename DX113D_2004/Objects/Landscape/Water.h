#pragma once

class Water : public Transform
{
private:
	class WaterBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float4 color = Float4(0.2f, 0.3f, 1.0f, 1.0f);

			float waveTranslation = 0.0f;
			float waveScale = 0.05f;
			float shininess = 30.0f;
			float alpha = 0.5f;

			float waveSpeed = 0.05f;
			float padding[3];
		}data;

		WaterBuffer() : ConstBuffer(&data, sizeof(Data))
		{
		}
	};

	WaterBuffer* buffer;
	float radius;

	Material* material;
	Mesh* mesh;

	UINT vertexCount, indexCount;

	Reflection* reflection;
	Refraction* refraction;

	BlendState* blendState[2];

public:
	Water(float radius = 30.0f);
	~Water();

	void Update();

	void SetReflection();
	void SetRefraction();
	void Render();
	void PostRender();

private:
	void CreateMesh();
};