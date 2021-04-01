#include "Framework.h"

GBuffer::GBuffer()
{
	diffuseRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	specularRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	depthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	rtvs[0] = diffuseRender;
	rtvs[1] = specularRender;
	rtvs[2] = normalRender;

	srvs[0] = depthStencil->GetSRV();
	srvs[1] = diffuseRender->GetSRV();
	srvs[2] = specularRender->GetSRV();
	srvs[3] = normalRender->GetSRV();

	for (UINT i = 0; i < 4; i++)
	{
		targetTextures[i] = new UIImage(L"Texture");
		targetTextures[i]->position = { 100 + (float)i * 200, 100, 0 };
		targetTextures[i]->scale = { 200, 200, 200 };
		targetTextures[i]->SetSRV(srvs[i]);
	}
}

GBuffer::~GBuffer()
{
	delete diffuseRender;
	delete specularRender;
	delete normalRender;
	delete depthStencil;

	for (UIImage* texture : targetTextures)
		delete texture;
}

void GBuffer::PreRender()
{
	RenderTarget::Sets(rtvs, 3, depthStencil);
}

void GBuffer::Render()
{
	DC->PSSetShaderResources(3, 1, &srvs[0]);
	DC->PSSetShaderResources(4, 1, &srvs[1]);
	DC->PSSetShaderResources(5, 1, &srvs[2]);
	DC->PSSetShaderResources(6, 1, &srvs[3]);
}

void GBuffer::PostRender()
{
	for (UIImage* texture : targetTextures)
		texture->Render();
}
