#pragma once

class RenderTarget
{
private:
	UINT width, height;

	ID3D11ShaderResourceView* srv;
	ID3D11Texture2D* rtvTexture;
	ID3D11RenderTargetView* rtv;

public:
	RenderTarget(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTarget();

	void SetDepthStencil(DepthStencil* depthStencil);
	static void Sets(RenderTarget** targets, UINT count, DepthStencil* depthStencil);

	ID3D11ShaderResourceView*& GetSRV() { return srv; }
	ID3D11RenderTargetView*& GetRTV() { return rtv; }
	ID3D11Texture2D* GetTexture() { return rtvTexture; }
};