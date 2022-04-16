#include "Framework.h"

GBuffer::GBuffer()
{
	mDiffuseRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSpecularRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mNormalRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	mRenderTargets.push_back(mDiffuseRenderTarget);
	mRenderTargets.push_back(mSpecularRenderTarget);
	mRenderTargets.push_back(mNormalRenderTarget);

	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	mSRVs[0] = mDepthStencil->GetSRV(); // ����Ÿ���� SRV.
	mSRVs[1] = mDiffuseRenderTarget->GetSRV();
	mSRVs[2] = mSpecularRenderTarget->GetSRV();
	mSRVs[3] = mNormalRenderTarget->GetSRV();

	for (UINT i = 0; i < 4; i++)
	{
		mTargetTextures[i] = new UIImage(L"Texture"); //UIImage �迭.
		mTargetTextures[i]->mPosition = { 100 + (float)i * 200, 100, 0 };
		mTargetTextures[i]->mScale = { 200, 200, 200 };
		mTargetTextures[i]->SetSRV(mSRVs[i]); // ��� srv(�̹���)
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
	RenderTarget::ClearAndSetWithDSV(mRenderTargets.data(), 3, mDepthStencil); // RTV�迭,RTV�迭 ������(����), depthStencil // OM�� SetRenderTarget
											   // OM�� MRT(MultiRenderTarget) Set.
}

void GBuffer::Render()
{
	DEVICECONTEXT->PSSetShaderResources(3, 1, &mSRVs[0]);
	DEVICECONTEXT->PSSetShaderResources(4, 1, &mSRVs[1]);
	DEVICECONTEXT->PSSetShaderResources(5, 1, &mSRVs[2]);
	DEVICECONTEXT->PSSetShaderResources(6, 1, &mSRVs[3]);
}

void GBuffer::PostRender()
{
	for (UIImage* texture : mTargetTextures)
		texture->Render();
}

// ����Ÿ���̶� DSV�Ѱ��ְ�,'�Ѱ��س���� SRV��' ���۵�����ü��̴��� ����. �׸��� �� SRV ����.
