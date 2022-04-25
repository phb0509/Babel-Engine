#include "Framework.h"

GBuffer::GBuffer()
{
	mDiffuseRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mNormalRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mDepthRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets.push_back(mDiffuseRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTarget);
	mRenderTargets.push_back(mNormalRenderTarget);
	mRenderTargets.push_back(mDepthRenderTarget);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	mSRVs[0] = mDepthStencil->GetSRV(); // 렌더타겟의 SRV.
	mSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mSRVs[2] = mSpecularRenderTarget->GetSRV();
	mSRVs[3] = mNormalRenderTarget->GetSRV();


	mShowSRVs[0] = mDepthRenderTarget->GetSRV(); // 렌더타겟의 SRV.
	mShowSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mShowSRVs[2] = mSpecularRenderTarget->GetSRV();
	mShowSRVs[3] = mNormalRenderTarget->GetSRV();

	for (UINT i = 0; i < 4; i++)
	{
		mTargetTextures[i] = new UIImage(L"Texture"); //UIImage 배열.
		mTargetTextures[i]->mPosition = { 100 + (float)i * 300, 100, 0 };
		mTargetTextures[i]->mScale = { 300.0f, 300.0f, 300.0f };
		mTargetTextures[i]->SetSRV(mShowSRVs[i]); // 띄울 srv(이미지)
	}
}

GBuffer::~GBuffer()
{
	delete mDiffuseRenderTarget;
	delete mSpecularRenderTarget;
	delete mNormalRenderTarget;
	delete mDepthStencil;

	for (UIImage* texture : mTargetTextures)
		delete texture;
}

void GBuffer::PreRender()
{
	RenderTarget::ClearAndSetWithDSV(mRenderTargets.data(), 4, mDepthStencil); // RTV배열,RTV배열 사이즈(개수), depthStencil // OM에 SetRenderTarget
											   // OM에 MRT(MultiRenderTarget) Set.
}

void GBuffer::SetRenderTargetsToPS()
{
	DEVICECONTEXT->PSSetShaderResources(3, 1, &mSRVs[0]); // 깊이
	DEVICECONTEXT->PSSetShaderResources(4, 1, &mSRVs[1]); // 디퓨즈
	DEVICECONTEXT->PSSetShaderResources(5, 1, &mSRVs[2]); // 스페큘라
	DEVICECONTEXT->PSSetShaderResources(6, 1, &mSRVs[3]); // 노말
}

void GBuffer::PostRender()
{
	for (UIImage* texture : mTargetTextures)
	{
		texture->Render();
	}
}

void GBuffer::ClearSRVs()
{
	ID3D11ShaderResourceView* pSRV = NULL;

	DEVICECONTEXT->PSSetShaderResources(3, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(4, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(5, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(6, 1, &pSRV);
}

// 렌더타겟이랑 DSV넘겨주고,'넘겨준놈들의 SRV를' 디퍼드라이팅셰이더에 셋팅. 그리고 그 SRV 렌더.
