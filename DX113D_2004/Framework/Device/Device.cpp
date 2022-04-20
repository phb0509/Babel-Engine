#include "Framework.h"
#include "Device.h"

Device::Device() :
	mbIsStencil(false)
{
	CreateDeviceAndSwapchain();
	CreateBackBuffer();
}

Device::~Device()
{
	mDevice->Release();
	mDeviceContext->Release();

	swapChain->Release();

	mRenderTargetView->Release();
	//mDSVtexture->Release();
}

void Device::CreateDeviceAndSwapchain()
{
	UINT width = WIN_WIDTH;
	UINT height = WIN_HEIGHT;

	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width; // 창 크기만큼 스왑체인설정.
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hWnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = true;

	V(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		//D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		&swapChain,
		&mDevice,
		nullptr,
		&mDeviceContext
	));		
}

void Device::CreateBackBuffer()
{
	ID3D11Texture2D* backBuffer; // 임시 버퍼.

	V(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)); // 스왑체인에서 백버퍼를 얻어낸다.
	V(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)); // 백버퍼랑 렌더타겟뷰 연결.
	backBuffer->Release();	// 필요없으니 해제.

	mRenderTargets.emplace_back(mRenderTargetView);
	
	//{//RenderTargetView Texture
	//	D3D11_TEXTURE2D_DESC desc = {};
	//	desc.Width = WIN_WIDTH;
	//	desc.Height = WIN_HEIGHT;
	//	desc.MipLevels = 1;
	//	desc.ArraySize = 1;
	//	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	desc.SampleDesc.Count = 1;
	//	desc.SampleDesc.Quality = 0;
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	//	V(mDevice->CreateTexture2D(&desc, nullptr, &mRTVtexture));
	//}

	//{// ShaderResourceView
	//	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	//	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;
	//	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	desc.Texture2D.MipLevels = 1;

	//	V(mDevice->CreateShaderResourceView(mRTVtexture, &desc, &mRTVsrv));
	//}

	// DepthStencilView 설정.

	{// DSV Texture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = WIN_WIDTH;
		desc.Height = WIN_HEIGHT;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//
		//desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		V(mDevice->CreateTexture2D(&desc, nullptr, &mDSVtexture));
	}

	{ // DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		V(mDevice->CreateDepthStencilView(mDSVtexture, &desc, &mDepthStencilView));
		mDSVtexture->Release();
	}

	
}

void Device::SetRenderTarget()
{
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
}

void Device::SetRenderTargetNullDSV()
{
	//mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
}

void Device::ClearRenderTargetView(Float4 color)
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, (float*)&color);
}

void Device::ClearDepthStencilView()
{
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Device::InitRenderTargets(RenderTarget** renderTargets, int count)
{
	for (UINT i = 0; i < count; i++)
	{
		mRenderTargets.push_back(renderTargets[i]->GetRTV());
	}
}

void Device::SetRenderTargets()
{
	float color[4] = { 0, 0, 0, 0 };

	if (mRenderTargets.size() != 0)
	{
		// All Clear

		for (UINT i = 0; i < mRenderTargets.size(); i++)
		{
			mDeviceContext->ClearRenderTargetView(mRenderTargets[i], color);
		}

		if (mDepthStencilView != nullptr)
		{
			mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}


		// All Set

		if (mDepthStencilView == nullptr)
		{
			mDeviceContext->OMSetRenderTargets(mRenderTargets.size(), mRenderTargets.data(), nullptr);
		}
		else
		{
			mDeviceContext->OMSetRenderTargets(mRenderTargets.size(), mRenderTargets.data(), mDepthStencilView);
		}
	}
	else
	{
		MessageBox(hWnd, L"There must be at least one render target.", L"Device.cpp", MB_ICONQUESTION | MB_OKCANCEL);
	}
	
}






void Device::Present()
{
	swapChain->Present(0, 0);
}

