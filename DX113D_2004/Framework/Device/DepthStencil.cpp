#include "Framework.h"

DepthStencil::DepthStencil(UINT width, UINT height, bool isStencil):
	mDSV(nullptr),
	mSRV(nullptr),
	mDSVTexture(nullptr)
{
	{//DSV Texture
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = isStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		
		V(DEVICE->CreateTexture2D(&texDesc, nullptr, &mDSVTexture));
	}

	{//DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = isStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//추가한것.
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = 0;

		V(DEVICE->CreateDepthStencilView(mDSVTexture, &dsvDesc, &mDSV));
	}

	{//SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = isStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		//추가한것
		srvDesc.Texture2D.MostDetailedMip = 0;

		V(DEVICE->CreateShaderResourceView(mDSVTexture, &srvDesc, &mSRV));
	}
}

DepthStencil::~DepthStencil()
{
	mDSVTexture->Release();
	mDSV->Release();
	mSRV->Release();
}

void DepthStencil::Clear()
{
	DEVICECONTEXT->ClearDepthStencilView(mDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
