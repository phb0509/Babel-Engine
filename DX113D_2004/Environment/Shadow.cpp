#include "Framework.h"

Shadow::Shadow(UINT width, UINT height) :
	mWidth(width), 
	mHeight(height), 
	mRadius(30)
{
	mRenderTarget = new RenderTarget(width, height);
	mDepthStencil = new DepthStencil(width, height);

	mDepthMap = new UIImage(L"Texture");
	mDepthMap->mScale = { 300, 300, 1 };
	mDepthMap->mPosition = { 150, 150, 0 };
	mDepthMap->SetSRV(mRenderTarget->GetSRV());

	mViewBuffer = new MatrixBuffer();
	mProjectionBuffer = new MatrixBuffer();

	mQualityBuffer = new TypeBuffer();
	mSizeBuffer = new SizeBuffer();
	mSizeBuffer->data.size = Float2(width, height);
}

Shadow::~Shadow()
{
	GM->SafeDelete(mRenderTarget);
	GM->SafeDelete(mDepthStencil);
	GM->SafeDelete(mDepthMap);
	GM->SafeDelete(mViewBuffer);
	GM->SafeDelete(mProjectionBuffer);
	GM->SafeDelete(mQualityBuffer);
	GM->SafeDelete(mSizeBuffer);
}

void Shadow::PreRender()
{
	mRenderTarget->SetDepthStencil(mDepthStencil);
	SetViewProjection();
}

void Shadow::Render()
{
	mViewBuffer->SetVSBuffer(11);
	mProjectionBuffer->SetVSBuffer(12);

	mQualityBuffer->SetPSBuffer(10);
	mSizeBuffer->SetPSBuffer(11);

	DEVICECONTEXT->PSSetShaderResources(10, 1, &mRenderTarget->GetSRV());
}

void Shadow::PostRender()
{
	mDepthMap->Render();

	ImGui::SliderFloat("Radius", &mRadius, 10.0f, 100.0f);
	ImGui::SliderInt("Quality", &mQualityBuffer->data.values[0], 0, 1);
}

void Shadow::SetViewProjection()
{
	Vector3 lightDir = mLight->GetLightInfo().direction;
	lightDir.Normalize();

	Vector3 lightPos = lightDir * -mRadius;

	Matrix view = XMMatrixLookAtLH(lightPos.data, XMVectorZero(), kUp);
	Matrix projection = XMMatrixOrthographicLH(mRadius * 2, mRadius * 2, 0.0f, 1000.0f);

	mViewBuffer->SetMatrix(view);
	mProjectionBuffer->SetMatrix(projection);

	mViewBuffer->SetVSBuffer(1);
	mProjectionBuffer->SetVSBuffer(2);
}
