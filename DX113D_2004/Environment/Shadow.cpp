#include "Framework.h"

Shadow::Shadow(UINT width, UINT height)
	: width(width), height(height), radius(30)
{
	renderTarget = new RenderTarget(width, height);
	depthStencil = new DepthStencil(width, height);

	depthMap = new UIImage(L"Texture");
	depthMap->mScale = { 300, 300, 1 };
	depthMap->mPosition = { 150, 150, 0 };
	depthMap->SetSRV(renderTarget->GetSRV());

	viewBuffer = new MatrixBuffer();
	projectionBuffer = new MatrixBuffer();

	qualityBuffer = new TypeBuffer();
	sizeBuffer = new SizeBuffer();
	sizeBuffer->data.size = Float2(width, height);
}

Shadow::~Shadow()
{
	delete renderTarget;
	delete depthStencil;

	delete depthMap;

	delete viewBuffer;
	delete projectionBuffer;
}

void Shadow::PreRender()
{
	renderTarget->SetDepthStencil(depthStencil);
	SetViewProjection();
}

void Shadow::Render()
{
	viewBuffer->SetVSBuffer(11);
	projectionBuffer->SetVSBuffer(12);

	qualityBuffer->SetPSBuffer(10);
	sizeBuffer->SetPSBuffer(11);

	DEVICECONTEXT->PSSetShaderResources(10, 1, &renderTarget->GetSRV());
}

void Shadow::PostRender()
{
	depthMap->Render();

	ImGui::SliderFloat("Radius", &radius, 10.0f, 100.0f);
	ImGui::SliderInt("Quality", &qualityBuffer->data.values[0], 0, 1);
}

void Shadow::SetViewProjection()
{
	Vector3 lightDir = Environment::Get()->GetLight()->data.lights[0].direction;
	lightDir.Normalize();

	Vector3 lightPos = lightDir * -radius;

	Matrix view = XMMatrixLookAtLH(lightPos.data, XMVectorZero(), kUp);
	Matrix projection = XMMatrixOrthographicLH(radius * 2, radius * 2, 0.0f, 1000.0f);

	viewBuffer->Set(view);
	projectionBuffer->Set(projection);

	viewBuffer->SetVSBuffer(1);
	projectionBuffer->SetVSBuffer(2);
}
