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


public:
	Scattering();
	~Scattering();

	void Update();
	void PreRender();
	void Render();
	void PostRener();
	void SetLight(Light* light) { mLight = light; }

private:
	void CreateQuad();
	void CreateSphere();


	TargetBuffer* mTargetBuffer;

	RenderTarget* mRenderTargets[2];
	DepthStencil* mDepthStencil;

	UIImage* mRayleighTexture;
	UIImage* mMieTexture;

	VertexUV* mQuadVertices;
	VertexBuffer* mQuadBuffer;

	Material* mQuadMaterial;

	//Shpere
	float mRadius;
	UINT mSlices, mStacks;
	UINT mVertexCount, mIndexCount;
	Mesh* mMesh;
	Material* mMaterial;

	TimeBuffer* mStarBuffer;

	DepthStencilState* mDepthMode[2];
	Light* mLight;
};