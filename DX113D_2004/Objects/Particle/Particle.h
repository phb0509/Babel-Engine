#pragma once

class Particle : public Transform
{
public:
	Particle(wstring diffuseFile);
	virtual ~Particle();

	virtual void Create() = 0;
	virtual void Update() = 0;
	virtual void Render();
	virtual void PostRender() = 0;
	virtual void Play(Vector3 position);
	virtual void Stop();

	void SetCamera(Camera* camera) { mCamera = camera; }

protected:
	const UINT MAX_COUNT = 1000;

	Material* material;
	GeometryShader* geometryShader;

	VertexBuffer* vertexBuffer;

	UINT particleCount;

	BlendState* blendState[2];
	DepthStencilState* depthState[2];
	Camera* mCamera;
};