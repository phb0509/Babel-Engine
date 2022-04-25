#pragma once

class DepthStencil
{
public:
	DepthStencil(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT, bool isStencil = false);	
	~DepthStencil();

	void Clear();

	ID3D11DepthStencilView*& GetDSV() { return mDSV; }
	ID3D11ShaderResourceView*& GetSRV() { return mSRV; }
	ID3D11Texture2D* GetTexture() { return mDSVTexture; }

private:
	ID3D11Texture2D* mDSVTexture;
	ID3D11DepthStencilView* mDSV;
	ID3D11ShaderResourceView* mSRV;
};