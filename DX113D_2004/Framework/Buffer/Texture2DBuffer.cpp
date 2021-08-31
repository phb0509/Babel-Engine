#include "Framework.h"

Texture2DBuffer::Texture2DBuffer(ID3D11Texture2D* src)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	mWidth = srcDesc.Width;
	mHeight = srcDesc.Height;
	mPage = srcDesc.ArraySize;
	mFormat = srcDesc.Format;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.ArraySize = mPage;
	desc.Format = mFormat;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture = nullptr;
	V(DEVICE->CreateTexture2D(&desc, nullptr, &texture)); 
	DEVICECONTEXT->CopyResource(texture, src);

	mInput = (ID3D11Resource*)texture;

	createSRV();
	createOutput();
	createUAV();
	createOutputSRV();
}

Texture2DBuffer::~Texture2DBuffer()
{

}

void Texture2DBuffer::createSRV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)mInput;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = mPage;

	V(DEVICE->CreateShaderResourceView(texture, &srvDesc, &mSRV)); 
	
}

void Texture2DBuffer::createOutput()
{
	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.ArraySize = mPage;
	desc.Format = mFormat;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	V(DEVICE->CreateTexture2D(&desc, nullptr, &texture)); 
	//DEVICE->CreateTexture2D(&desc, NULL, &texture); // 여기에러

	mOutput = (ID3D11Resource*)texture;
}

void Texture2DBuffer::createUAV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)mOutput;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	//uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.ArraySize = mPage;
	V(DEVICE->CreateUnorderedAccessView(texture, &uavDesc, &mUAV));

	
}

void Texture2DBuffer::createOutputSRV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)mOutput;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = mPage;
	
	V(DEVICE->CreateShaderResourceView(texture, &srvDesc, &mOutputSRV));
}