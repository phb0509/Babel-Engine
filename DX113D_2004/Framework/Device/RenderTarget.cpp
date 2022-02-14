#include "Framework.h"

RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format) // 툴씬에선 R8G8B8A8 UNORM (0~1로 사상되는 타입)
	: 
	mRTV(nullptr),
	mRTVtextureForCopy(nullptr),
	mSRV(nullptr),
	mWidth(width), 
	mHeight(height)
{
	{//RenderTargetView Texture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		V(DEVICE->CreateTexture2D(&desc, nullptr, &mRTVtexture));
	}

	{//RenderTargetView
		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		V(DEVICE->CreateRenderTargetView(mRTVtexture, &desc, &mRTV));
	}

	{// ShaderResourceView
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		V(DEVICE->CreateShaderResourceView(mRTVtexture, &desc, &mSRV));
	}
}

RenderTarget::~RenderTarget()
{
	mRTVtexture->Release();
	mSRV->Release();
	mRTV->Release();
	mRTVtextureForCopy->Release();
}

void RenderTarget::SetDepthStencil(DepthStencil* depthStencil)
{
	DEVICECONTEXT->OMSetRenderTargets(1, &mRTV, depthStencil->GetDSV());
	
	Float4 color = { 0, 0, 0, 0 };
	DEVICECONTEXT->ClearRenderTargetView(mRTV, (float*)&color);
	depthStencil->Clear();

	Environment::Get()->SetViewport(mWidth, mHeight);
}

void RenderTarget::Sets(RenderTarget** targets, UINT count, DepthStencil* depthStencil)
{
	vector<ID3D11RenderTargetView*> rtvs;

	float color[4] = { 0, 0, 0, 0 };
	
	for (UINT i = 0; i < count; i++)
	{
		rtvs.emplace_back(targets[i]->GetRTV());
		DEVICECONTEXT->ClearRenderTargetView(targets[i]->GetRTV(), color);
	}

	depthStencil->Clear();

	//ID3D11RenderTargetView* pNullRTV = NULL;
	//DEVICECONTEXT->OMSetRenderTargets(1, &pNullRTV, NULL);//<--- 텍스처를 바인딩하고 렌더 대상으로 바인딩할 수 없기 때문에 이것이 필요합니다. 먼저 출시해야
	
	//전부 클리어 후, Set하기.
	DEVICECONTEXT->OMSetRenderTargets(rtvs.size(), rtvs.data(), depthStencil->GetDSV());
}
