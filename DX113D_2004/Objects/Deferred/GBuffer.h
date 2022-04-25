#pragma once

class GBuffer
{

public:

	GBuffer();
	~GBuffer();

	void PreRender();
	void SetRenderTargetsToPS();
	void PostRender();
	vector<RenderTarget*> GetRenderTargets() { return mRenderTargets; }
	DepthStencil* GetDepthStencil() { return mDepthStencil; }
	void ClearSRVs();

private:

	RenderTarget* mDiffuseRenderTarget;
	RenderTarget* mSpecularRenderTarget;
	RenderTarget* mNormalRenderTarget;
	RenderTarget* mDepthRenderTarget;
	DepthStencil* mDepthStencil;
	vector<RenderTarget*> mRenderTargets;
	UIImage* mTargetTextures[4];
	ID3D11ShaderResourceView* mSRVs[4];
	ID3D11ShaderResourceView* mShowSRVs[4];

};