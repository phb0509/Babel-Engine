#pragma once

class GBuffer
{
private:
	RenderTarget* diffuseRender;
	RenderTarget* specularRender;
	RenderTarget* normalRender;

	DepthStencil* depthStencil;

	RenderTarget* rtvs[3];

	UIImage* targetTextures[4];

	ID3D11ShaderResourceView* srvs[4];
public:
	GBuffer();
	~GBuffer();

	void PreRender();
	void Render();
	void PostRender();
};