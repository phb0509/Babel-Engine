#include "Framework.h"

GBuffer::GBuffer()
{
	diffuseRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	specularRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	depthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	rtvs[0] = diffuseRender; // 렌더타겟임.
	rtvs[1] = specularRender;
	rtvs[2] = normalRender;

	srvs[0] = depthStencil->GetSRV(); // 렌더타겟의 SRV.
	srvs[1] = diffuseRender->GetSRV();
	srvs[2] = specularRender->GetSRV();
	srvs[3] = normalRender->GetSRV();

	

	for (UINT i = 0; i < 4; i++)
	{
		targetTextures[i] = new UIImage(L"Texture"); //UIImage 배열.
		targetTextures[i]->mPosition = { 100 + (float)i * 200, 100, 0 };
		targetTextures[i]->mScale = { 200, 200, 200 };
		targetTextures[i]->SetSRV(srvs[i]); // 띄울 srv(이미지)
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
	RenderTarget::Sets(rtvs, 3, depthStencil); // RTV배열,RTV배열 사이즈(개수), depthStencil // OM에 SetRenderTarget
											   // OM에 MRT(MultiRenderTarget) Set.
}

void GBuffer::Render()
{
	DEVICECONTEXT->PSSetShaderResources(3, 1, &srvs[0]);
	DEVICECONTEXT->PSSetShaderResources(4, 1, &srvs[1]);
	DEVICECONTEXT->PSSetShaderResources(5, 1, &srvs[2]);
	DEVICECONTEXT->PSSetShaderResources(6, 1, &srvs[3]);
}

void GBuffer::PostRender()
{
	for (UIImage* texture : targetTextures)
		texture->Render();
}

// 렌더타겟이랑 DSV넘겨주고,'넘겨준놈들의 SRV를' 디퍼드라이팅셰이더에 셋팅. 그리고 그 SRV 렌더.
