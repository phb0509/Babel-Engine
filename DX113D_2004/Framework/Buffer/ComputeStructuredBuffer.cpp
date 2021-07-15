#include "Framework.h"

ComputeStructuredBuffer::ComputeStructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
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

ComputeStructuredBuffer::~ComputeStructuredBuffer()
{
	input->Release();
	srv->Release();
	output->Release();
	uav->Release();
	result->Release();
}

void ComputeStructuredBuffer::Copy(void* data, UINT size)
{
	DEVICECONTEXT->CopyResource(result, output);

	D3D11_MAPPED_SUBRESOURCE subResource;

	DEVICECONTEXT->Map(result, 0, D3D11_MAP_READ, 0, &subResource);
	memcpy(data, subResource.pData, size);
	DEVICECONTEXT->Unmap(result, 0);
}

void ComputeStructuredBuffer::CreateInput() // 계산셰이더로 넘길 버퍼 설정.
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

	input = (ID3D11Resource*)buffer; // ID3D11Buffer* -> ID3D11Resource*
}

void ComputeStructuredBuffer::CreateSRV() // 읽기전용자원.'gpu에서' 읽기전용자원으로 쓰인다. ex)Texture
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input; // ID3D11Resource  ->  ID3D11Buffer*

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = inputCount;

	V(DEVICE->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void ComputeStructuredBuffer::CreateOutput() // 계산셰이더에서 계산된 값을 CPU에서 넘겨받을 버퍼설정.
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = outputStride * outputCount; // 데이터타입크기 * 개수
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UAV로 설정.
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;	// 데이터타입크기(버퍼크기)

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	output = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::CreateUAV() // gpu에서 계산한 데이터를 다시 담아서 cpu로 가져오기위한 view (adaptor같은?) / 계산된 
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

void ComputeStructuredBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;	// 값을 복사해주기 위해 Staging
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // 계산셰이더에서 넘어온 값을 CPU가 읽을 수 있도록.

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	result = (ID3D11Resource*)buffer;
}
