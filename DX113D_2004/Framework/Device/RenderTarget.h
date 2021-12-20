#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTarget();

	void SetDepthStencil(DepthStencil* depthStencil);
	static void Sets(RenderTarget** targets, UINT count, DepthStencil* depthStencil);

	ID3D11ShaderResourceView*& GetSRV() { return mSRV; }
	ID3D11RenderTargetView*& GetRTV() { return mRTV; }
	ID3D11Texture2D* GetTexture() { return mRTVtexture; }
	ID3D11Texture2D* GetTextureForCop() { return mRTVtextureForCopy; }

private:
	UINT mWidth;
	UINT mHeight;

	ID3D11ShaderResourceView* mSRV;
	ID3D11Texture2D* mRTVtexture;
	ID3D11Texture2D* mRTVtextureForCopy;
	ID3D11RenderTargetView* mRTV;
};