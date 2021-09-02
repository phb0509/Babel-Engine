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

ComputeStructuredBuffer::ComputeStructuredBuffer(UINT outputStride, UINT outputCount): // 구조체크기, 개수
	outputStride(outputStride),
	outputCount(outputCount)
{
	CreateOutput();
	CreateUAV();
	CreateResult();
}

ComputeStructuredBuffer::~ComputeStructuredBuffer()
{
	if (input != nullptr)
	{
		input->Release();
	}

	if (srv != nullptr)
	{
		srv->Release();
	}
	
	output->Release();
	uav->Release();
	result->Release();
}


void ComputeStructuredBuffer::CreateInput() // 계산셰이더로 넘길 버퍼 설정,생성.
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

void ComputeStructuredBuffer::CreateSRV() // 읽기전용자원.'gpu에서' 읽기전용자원으로 쓰인다. ex)Texture, 버퍼를 셰이더에 넘기기위해 srv로 변환.
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input; // ID3D11Resource  ->  ID3D11Buffer*

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc); // input의 설정을 desc에 저장. 근데 


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = inputCount;

	V(DEVICE->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void ComputeStructuredBuffer::CreateOutput() // 계산셰이더에서 계산된 값을 CPU에서 넘겨받을 UAV버퍼설정.
{
	ID3D11Buffer* buffer;
	
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = outputStride * outputCount; // 데이터타입크기 * 개수
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UAV로 설정.
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;	// 데이터타입크기(버퍼크기)

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer)); // buffer에 desc설정 담기.

	output = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::CreateUAV() // gpu에서 계산한 데이터를 다시 담아서 cpu로 가져오기위한 view / output버퍼 uav로 변환.
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

void ComputeStructuredBuffer::CreateResult() // 데이터카피하기위한 버퍼생성.
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)output)->GetDesc(&desc); //아웃풋버퍼설정 desc로 복사. 당연히 같아야하니까..

	desc.Usage = D3D11_USAGE_STAGING;	// 값을 복사받기 위해 Staging
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // 계산셰이더에서 넘어온 값을 CPU가 읽을 수 있도록.

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	result = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::Copy(void* data, UINT size) // 구조체, 받아와야할 전체 크기 (구조체크기 * 개수) , 실제데이터 복사하는 과정.
{
	DEVICECONTEXT->CopyResource(result, output); //output에서 result로 카피.

	D3D11_MAPPED_SUBRESOURCE subResource;

	DEVICECONTEXT->Map(result, 0, D3D11_MAP_READ, 0, &subResource); // subresource에 컴퓨트셰이더에서 나온 결과값들메모리의 주소 따와서 저장.
	memcpy(data, subResource.pData, size); // subResource.pData부터 size만큼의 크기를 data로 복사.
	DEVICECONTEXT->Unmap(result, 0);
}