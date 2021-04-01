#pragma once

class Shadow
{
private:
	UINT width, height;
	float radius;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;

	class UIImage* depthMap;

	MatrixBuffer* viewBuffer;
	MatrixBuffer* projectionBuffer;

	TypeBuffer* qualityBuffer;
	SizeBuffer* sizeBuffer;
public:
	Shadow(UINT width = 2000, UINT height = 2000);
	~Shadow();

	void PreRender();
	void Render();
	void PostRender();

	void SetViewProjection();
};