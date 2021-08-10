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
	device->Release();
	deviceContext->Release();

	swapChain->Release();

	renderTargetView->Release();
	mDSVtexture->Release();
	
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
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		&swapChain,
		&device,
		nullptr,
		&deviceContext
	));		
}

void Device::CreateBackBuffer()
{
	ID3D11Texture2D* backBuffer; // 임시 버퍼.

	V(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)); // 스왑체인에서 백버퍼를 얻어낸다.
	V(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView)); // 백버퍼랑 렌더타겟뷰 연결.
	backBuffer->Release();	// 필요없으니 해제.


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

		V(device->CreateTexture2D(&desc, nullptr, &mDSVtexture));
	}

	{ // DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		V(device->CreateDepthStencilView(mDSVtexture, &desc, &depthStencilView));
		mDSVtexture->Release();
	}

	//{//SRV
	//	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	//	desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//	//desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	desc.Texture2D.MipLevels = 1;
	//	//V(device->CreateShaderResourceView(mDSVtexture, &desc, &mDSVsrv));
	//	device->CreateShaderResourceView(mDSVtexture, &desc, &mDSVsrv);

	//}
}

void Device::SetRenderTarget()
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Device::Clear(Float4 color)
{
	deviceContext->ClearRenderTargetView(renderTargetView, (float*)&color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
}

void Device::Present()
{
	swapChain->Present(0, 0);
}
