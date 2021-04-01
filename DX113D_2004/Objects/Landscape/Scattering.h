#pragma once

class Scattering
{
private:
	class TargetBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float3 waveLength = Float3(0.65f, 0.57f, 0.475f);
			int sampleCount = 20;

			Float3 invWaveLength;
			float padding;

			Float3 waveLengthMie;
			float padding2;			
		}data;

		TargetBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.invWaveLength.x = 1.0f / pow(data.waveLength.x, 4.0f);
			data.invWaveLength.y = 1.0f / pow(data.waveLength.y, 4.0f);
			data.invWaveLength.z = 1.0f / pow(data.waveLength.z, 4.0f);

			data.waveLengthMie.x = pow(data.waveLength.x, 0.84f);
			data.waveLengthMie.y = pow(data.waveLength.y, 0.84f);
			data.waveLengthMie.z = pow(data.waveLength.z, 0.84f);
		}
	};

	TargetBuffer* targetBuffer;

	RenderTarget* targets[2];
	DepthStencil* depthStencil;

	UIImage* rayleighTexture;
	UIImage* mieTexture;

	VertexUV* quadVertices;
	VertexBuffer* quadBuffer;

	Material* quadMaterial;

	//Shpere
	float radius;

	UINT slices, stacks;

	UINT vertexCount, indexCount;
	Mesh* mesh;
	Material* material;

	TimeBuffer* starBuffer;

	DepthStencilState* depthMode[2];
public:
	Scattering();
	~Scattering();

	void Update();
	void PreRender();
	void Render();
	void PostRener();

private:
	void CreateQuad();
	void CreateSphere();
};