#include "Framework.h"

Reflection::Reflection(Transform* transform)
	: transform(transform)
{
	renderTarget = new RenderTarget(2000, 2000);
	depthStencil = new DepthStencil(2000, 2000);

	reflectionBuffer = new MatrixBuffer();

	targetTexture = new UIImage(L"Texture");
	targetTexture->SetSRV(renderTarget->GetSRV());
	targetTexture->scale = { 300, 300, 1 };
	targetTexture->position = { 150, 150, 0 };

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
	camera->rotation = CAMERA->rotation;
	camera->position = CAMERA->position;

	camera->rotation.x *= -1.0f;
	camera->position.y = transform->position.y * 2.0f - camera->position.y;

	camera->View();

	reflectionBuffer->Set(camera->GetView());
}

void Reflection::PreRender()
{
	renderTarget->Set(depthStencil);
	reflectionBuffer->SetVSBuffer(1);
}

void Reflection::Render()
{
	DC->PSSetShaderResources(10, 1, &renderTarget->GetSRV());
	reflectionBuffer->SetVSBuffer(10);
}

void Reflection::PostRender()
{
	targetTexture->Render();
}
