#pragma once

class GBuffer
{

public:

	GBuffer();
	~GBuffer();

	void PreRender();
	void Render();
	void PostRender();
	vector<RenderTarget*> GetRenderTargets() { return mRenderTargets; }
	DepthStencil* GetDepthStencil() { return mDepthStencil; }

private:

	RenderTarget* mDiffuseRenderTarget;
	RenderTarget* mSpecularRenderTarget;
	RenderTarget* mNormalRenderTarget;
	DepthStencil* mDepthStencil;
	vector<RenderTarget*> mRenderTargets;
	UIImage* mTargetTextures[4];
	ID3D11ShaderResourceView* mSRVs[4];

};