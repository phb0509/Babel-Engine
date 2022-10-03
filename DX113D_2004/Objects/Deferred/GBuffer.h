#pragma once

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void ClearAndSetRenderTargets();
	void SetTexturesToPS();
	void PostRender();
	vector<RenderTarget*> GetRenderTargets() { return mRenderTargets; }
	DepthStencil* GetDepthStencil() { return mDepthStencil; }
	void ClearSRVs();

private:
	RenderTarget* mDiffuseRenderTarget;
	RenderTarget* mSpecularRenderTarget;
	RenderTarget* mNormalRenderTarget;
	RenderTarget* mDepthRenderTarget;
	RenderTarget* mSpecularRenderTargetForShow;
	RenderTarget* mWorldPositionRenderTarget;

	DepthStencil* mDepthStencil;
	vector<RenderTarget*> mRenderTargets;
	ID3D11ShaderResourceView* mSRVs[5];
	ID3D11ShaderResourceView* mShowSRVs[5];
};