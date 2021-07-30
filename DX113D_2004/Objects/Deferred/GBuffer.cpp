#include "Framework.h"

GBuffer::GBuffer()
{
	diffuseRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	specularRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRender = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	depthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true);

	rtvs[0] = diffuseRender; // ����Ÿ����.
	rtvs[1] = specularRender;
	rtvs[2] = normalRender;

	srvs[0] = depthStencil->GetSRV(); // ����Ÿ���� SRV.
	srvs[1] = diffuseRender->GetSRV();
	srvs[2] = specularRender->GetSRV();
	srvs[3] = normalRender->GetSRV();

	

	for (UINT i = 0; i < 4; i++)
	{
		targetTextures[i] = new UIImage(L"Texture"); //UIImage �迭.
		targetTextures[i]->mPosition = { 100 + (float)i * 200, 100, 0 };
		targetTextures[i]->mScale = { 200, 200, 200 };
		targetTextures[i]->SetSRV(srvs[i]); // ��� srv(�̹���)
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
	RenderTarget::Sets(rtvs, 3, depthStencil); // RTV�迭,RTV�迭 ������(����), depthStencil // OM�� SetRenderTarget
											   // OM�� MRT(MultiRenderTarget) Set.
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

// ����Ÿ���̶� DSV�Ѱ��ְ�,'�Ѱ��س���� SRV��' ���۵�����ü��̴��� ����. �׸��� �� SRV ����.
