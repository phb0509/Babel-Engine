#include "Framework.h"

StructuredBuffer::StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
	: inputData(inputData), inputStride(inputStride), inputCount(inputCount),
	outputStride(outputStride), outputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->outputStride = inputStride;
		this->outputCount = inputCount;
	}

	CreateInput();
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

StructuredBuffer::~StructuredBuffer()
{
	input->Release();
	srv->Release();
	output->Release();
	uav->Release();
	result->Release();
}

void StructuredBuffer::Copy(void* data, UINT size)
{
	DEVICECONTEXT->CopyResource(result, output);

	D3D11_MAPPED_SUBRESOURCE subResource;

	DEVICECONTEXT->Map(result, 0, D3D11_MAP_READ, 0, &subResource);
	memcpy(data, subResource.pData, size);
	DEVICECONTEXT->Unmap(result, 0);
}

void StructuredBuffer::CreateInput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = inputStride * inputCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = inputStride;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = inputData;

	V(DEVICE->CreateBuffer(&desc, &initData, &buffer));

	input = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateSRV() // 읽기전용자원.gpu에서느 읽기전용자원으로 쓰인다. ex)Texture
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = inputCount;

	V(DEVICE->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void StructuredBuffer::CreateOutput()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = outputStride * outputCount;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;	

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	output = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateUAV() // gpu에서 계산한 데이터를 다시 담아서 cpu로 가져오기위한 view (adaptor같은?) / 계산된 
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = outputCount;

	V(DEVICE->CreateUnorderedAccessView(buffer, &uavDesc, &uav));
}

void StructuredBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;	
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	result = (ID3D11Resource*)buffer;
}
