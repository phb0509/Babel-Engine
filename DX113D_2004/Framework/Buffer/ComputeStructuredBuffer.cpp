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

ComputeStructuredBuffer::ComputeStructuredBuffer(UINT outputStride, UINT outputCount): // ����üũ��, ����
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


void ComputeStructuredBuffer::CreateInput() // �����̴��� �ѱ� ���� ����,����.
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

void ComputeStructuredBuffer::CreateSRV() // �б������ڿ�.'gpu����' �б������ڿ����� ���δ�. ex)Texture, ���۸� ���̴��� �ѱ������ srv�� ��ȯ.
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input; // ID3D11Resource  ->  ID3D11Buffer*

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc); // input�� ������ desc�� ����. �ٵ� 


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = inputCount;

	V(DEVICE->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void ComputeStructuredBuffer::CreateOutput() // �����̴����� ���� ���� CPU���� �Ѱܹ��� UAV���ۼ���.
{
	ID3D11Buffer* buffer;
	
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = outputStride * outputCount; // ������Ÿ��ũ�� * ����
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UAV�� ����.
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;	// ������Ÿ��ũ��(����ũ��)

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer)); // buffer�� desc���� ���.

	output = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::CreateUAV() // gpu���� ����� �����͸� �ٽ� ��Ƽ� cpu�� ������������ view / output���� uav�� ��ȯ.
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

void ComputeStructuredBuffer::CreateResult() // ������ī���ϱ����� ���ۻ���.
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)output)->GetDesc(&desc); //�ƿ�ǲ���ۼ��� desc�� ����. �翬�� ���ƾ��ϴϱ�..

	desc.Usage = D3D11_USAGE_STAGING;	// ���� ����ޱ� ���� Staging
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // �����̴����� �Ѿ�� ���� CPU�� ���� �� �ֵ���.

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));

	result = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::Copy(void* data, UINT size) // ����ü, �޾ƿ;��� ��ü ũ�� (����üũ�� * ����) , ���������� �����ϴ� ����.
{
	DEVICECONTEXT->CopyResource(result, output); //output���� result�� ī��.

	D3D11_MAPPED_SUBRESOURCE subResource;

	DEVICECONTEXT->Map(result, 0, D3D11_MAP_READ, 0, &subResource); // subresource�� ��ǻƮ���̴����� ���� �������޸��� �ּ� ���ͼ� ����.
	memcpy(data, subResource.pData, size); // subResource.pData���� size��ŭ�� ũ�⸦ data�� ����.
	DEVICECONTEXT->Unmap(result, 0);
}