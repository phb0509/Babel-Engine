#include "Framework.h"

Refraction::Refraction(wstring normalFile)
{
	mTimeBuffer = new TimeBuffer();

	mNormalMap = Texture::Add(normalFile);

	mRenderTarget = new RenderTarget(2000, 2000);
	mDepthStencil = new DepthStencil(2000, 2000);

	mTargetTexture = new UIImage(L"Texture");
	mTargetTexture->SetSRV(mRenderTarget->GetSRV());
	mTargetTexture->mScale = { 300, 300, 1 };
	mTargetTexture->mPosition = { 150, 150, 0 };
}

Refraction::~Refraction()
{
	delete mTimeBuffer;
	delete mRenderTarget;
	delete mDepthStencil;
	delete mTargetTexture;
}

void Refraction::Update()
{
	mTimeBuffer->data.time += DELTA;
}

void Refraction::PreRender()
{
	mRenderTarget->SetDepthStencil(mDepthStencil);
}

void Refraction::Render()
{
	mTimeBuffer->SetPSBuffer(11);
	mNormalMap->PSSet(2);
	DEVICECONTEXT->PSSetShaderResources(11, 1, &mRenderTarget->GetSRV());
}

void Refraction::PostRender()
{
	mTargetTexture->Render();
}
