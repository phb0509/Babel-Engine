#pragma once

class Reflection
{
private:
	Transform* transform;

	MatrixBuffer* reflectionBuffer;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;

	Camera* camera;

	class UIImage* targetTexture;

public:
	Reflection(Transform* transform);
	~Reflection();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
};