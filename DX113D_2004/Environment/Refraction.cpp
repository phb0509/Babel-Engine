#include "Framework.h"

Refraction::Refraction(wstring normalFile)
{
	timeBuffer = new TimeBuffer();

	normalMap = Texture::Add(normalFile);

	renderTarget = new RenderTarget(2000, 2000);
	depthStencil = new DepthStencil(2000, 2000);

	targetTexture = new UIImage(L"Texture");
	targetTexture->SetSRV(renderTarget->GetSRV());
	targetTexture->mScale = { 300, 300, 1 };
	targetTexture->mPosition = { 150, 150, 0 };
}

Refraction::~Refraction()
{
	delete timeBuffer;
	delete renderTarget;
	delete depthStencil;
	delete targetTexture;
}

void Refraction::Update()
{
	timeBuffer->data.time += DELTA;
}

void Refraction::PreRender()
{
	renderTarget->Set(depthStencil);
}

void Refraction::Render()
{
	timeBuffer->SetPSBuffer(11);
	normalMap->PSSet(2);
	DC->PSSetShaderResources(11, 1, &renderTarget->GetSRV());
}

void Refraction::PostRender()
{
	targetTexture->Render();
}
