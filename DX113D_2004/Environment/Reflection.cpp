#include "Framework.h"

Reflection::Reflection(Transform* transform)
	: transform(transform)
{
	renderTarget = new RenderTarget(2000, 2000);
	depthStencil = new DepthStencil(2000, 2000);

	reflectionBuffer = new MatrixBuffer();

	targetTexture = new UIImage(L"Texture");
	targetTexture->SetSRV(renderTarget->GetSRV());
	targetTexture->mScale = { 300, 300, 1 };
	targetTexture->mPosition = { 150, 150, 0 };

	camera = new Camera();
}

Reflection::~Reflection()
{
	delete renderTarget;
	delete depthStencil;
	delete reflectionBuffer;
	delete targetTexture;
	delete camera;
}

void Reflection::Update()
{	
	camera->mRotation = TARGETCAMERA->mRotation;
	camera->mPosition = TARGETCAMERA->mPosition;

	camera->mRotation.x *= -1.0f;
	camera->mPosition.y = transform->mPosition.y * 2.0f - camera->mPosition.y;

	camera->SetViewMatrixToBuffer();

	reflectionBuffer->Set(camera->GetViewMatrix());
}

void Reflection::PreRender()
{
	renderTarget->SetDepthStencil(depthStencil);  // ¼ÎÀÌ´õ¿¡ ³Ñ±æ ·»´õÅ¸°Ù Àâ´ÂºÎºĞ.
	reflectionBuffer->SetVSBuffer(1);
}

void Reflection::Render()
{
	DEVICECONTEXT->PSSetShaderResources(10, 1, &renderTarget->GetSRV()); // ¼ÎÀÌ´õ¿¡ ¼ÂÆÃÇÒ ºÎºĞ.
	reflectionBuffer->SetVSBuffer(10);
}

void Reflection::PostRender()
{
	targetTexture->Render();
}
