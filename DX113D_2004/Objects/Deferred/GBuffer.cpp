#include "Framework.h"

GBuffer::GBuffer()
{
	mDiffuseRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mNormalRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mDepthRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTargetForShow = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets.push_back(mDiffuseRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTarget);
	mRenderTargets.push_back(mNormalRenderTarget);
	mRenderTargets.push_back(mDepthRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTargetForShow);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	mSRVs[0] = mDepthStencil->GetSRV(); // 렌더타겟의 SRV.
	mSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mSRVs[2] = mSpecularRenderTarget->GetSRV();
	mSRVs[3] = mNormalRenderTarget->GetSRV();

	mShowSRVs[0] = mDepthRenderTarget->GetSRV(); // 렌더타겟의 SRV.
	mShowSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mShowSRVs[2] = mSpecularRenderTargetForShow->GetSRV();
	mShowSRVs[3] = mNormalRenderTarget->GetSRV();

	for (UINT i = 0; i < 4; i++)
	{
		mTargetTextures[i] = new UIImage(L"Texture"); //UIImage 배열.
		mTargetTextures[i]->SetSRV(mShowSRVs[i]); // 띄울 srv(이미지)
		mTargetTextures[i]->mScale = { 200.0f, 200.0f, 0.0f };
	}

	float textureSize = 200.0f; // 
	float offsetY = 100.0f;
	float offsetX = 0.0f;

	mTargetTextures[0]->mPosition = { 100.0f, WIN_HEIGHT / 2 + textureSize / 2 + offsetY, 0 }; // depth
	mTargetTextures[1]->mPosition = { 300.0f, WIN_HEIGHT / 2 + textureSize / 2 + offsetY, 0 }; // diffuse
	mTargetTextures[2]->mPosition = { 100.0f, WIN_HEIGHT / 2 - textureSize / 2 + offsetY, 0 }; // specular
	mTargetTextures[3]->mPosition = { 300.0f, WIN_HEIGHT / 2 - textureSize / 2 + offsetY, 0 }; // normal
}

GBuffer::~GBuffer()
{
	GM->SafeDelete(mDiffuseRenderTarget);
	GM->SafeDelete(mSpecularRenderTarget);
	GM->SafeDelete(mNormalRenderTarget);
	GM->SafeDelete(mDepthStencil);
	GM->SafeDeleteArray(mTargetTextures);
}

void GBuffer::PreRender()
{
	RenderTarget::ClearAndSetWithDSV(mRenderTargets.data(), 5, mDepthStencil); // RTV배열,RTV배열 사이즈(개수), depthStencil // OM에 SetRenderTarget	   
}

void GBuffer::SetRenderTargetsToPS()
{
	DEVICECONTEXT->PSSetShaderResources(10, 1, &mSRVs[0]); // 깊이
	DEVICECONTEXT->PSSetShaderResources(11, 1, &mSRVs[1]); // 디퓨즈
	DEVICECONTEXT->PSSetShaderResources(12, 1, &mSRVs[2]); // 스페큘라
	DEVICECONTEXT->PSSetShaderResources(13, 1, &mSRVs[3]); // 노말
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

	DEVICECONTEXT->PSSetShaderResources(10, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(11, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(12, 1, &pSRV);
	DEVICECONTEXT->PSSetShaderResources(13, 1, &pSRV);
}


