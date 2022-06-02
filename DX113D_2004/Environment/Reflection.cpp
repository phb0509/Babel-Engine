#include "Framework.h"

Reflection::Reflection(Transform* transform)
	: mTransform(transform)
{
	mRenderTarget = new RenderTarget(2000, 2000);
	mDepthStencil = new DepthStencil(2000, 2000);

	mReflectionBuffer = new MatrixBuffer();

	mTargetTexture = new UIImage(L"Texture");
	mTargetTexture->SetSRV(mRenderTarget->GetSRV());
	mTargetTexture->mScale = { 300, 300, 1 };
	mTargetTexture->mPosition = { 150, 150, 0 };

	mWorldCamera = new Camera();
}

Reflection::~Reflection()
{
	delete mRenderTarget;
	delete mDepthStencil;
	delete mReflectionBuffer;
	delete mTargetTexture;
	delete mWorldCamera;
}

void Reflection::Update()
{	
	mWorldCamera->mRotation = mTargetCamera->mRotation;
	mWorldCamera->mPosition = mTargetCamera->mPosition;

	mWorldCamera->mRotation.x *= -1.0f;
	mWorldCamera->mPosition.y = mTransform->mPosition.y * 2.0f - mWorldCamera->mPosition.y;

	mWorldCamera->SetViewMatrixToBuffer();

	mReflectionBuffer->SetMatrix(mWorldCamera->GetViewMatrix());
}

void Reflection::PreRender()
{
	mRenderTarget->SetDepthStencil(mDepthStencil);  // ¼ÎÀÌ´õ¿¡ ³Ñ±æ ·»´õÅ¸°Ù Àâ´ÂºÎºĞ.
	mReflectionBuffer->SetVSBuffer(1);
}

void Reflection::Render()
{
	DEVICECONTEXT->PSSetShaderResources(10, 1, &mRenderTarget->GetSRV()); // ¼ÎÀÌ´õ¿¡ ¼ÂÆÃÇÒ ºÎºĞ.
	mReflectionBuffer->SetVSBuffer(10);
}

void Reflection::PostRender()
{
	mTargetTexture->Render();
}
