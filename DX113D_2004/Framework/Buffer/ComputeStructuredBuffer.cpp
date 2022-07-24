#include "Framework.h"

ComputeStructuredBuffer::ComputeStructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount) : 
	mInputData(inputData), 
	mInputStride(inputStride), 
	mInputCount(inputCount),
	mOutputStride(outputStride), 
	mOutputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->mOutputStride = inputStride;
		this->mOutputCount = inputCount;
	}

	CreateInput();
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}

ComputeStructuredBuffer::ComputeStructuredBuffer(UINT outputStride, UINT outputCount): // ����üũ��, ����
	mOutputStride(outputStride),
	mOutputCount(outputCount)
{
	CreateOutput();
	CreateUAV();
	CreateResult();
}

ComputeStructuredBuffer::~ComputeStructuredBuffer()
{
	mSRV->Release();
	
	mUAVbuffer->Release();
	mUAV->Release();
	mReturnedDataFromGPU->Release();
}


void ComputeStructuredBuffer::CreateInput() // �����̴��� �ѱ� ���� ����,����.
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = mInputStride * mInputCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = mInputStride;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = mInputData;

	V(DEVICE->CreateBuffer(&desc, &initData, &buffer));

	mInput = (ID3D11Resource*)buffer; // ID3D11Buffer* -> ID3D11Resource*
}

void ComputeStructuredBuffer::CreateSRV() // �б������ڿ�.'gpu����' �б������ڿ����� ���δ�. ex)Texture, ���۸� ���̴��� �ѱ������ srv�� ��ȯ.
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)mInput; // ID3D11Resource  ->  ID3D11Buffer*

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc); // input�� ������ desc�� ����. �ٵ� 


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = mInputCount;

	V(DEVICE->CreateShaderResourceView(buffer, &srvDesc, &mSRV));
}

void ComputeStructuredBuffer::CreateOutput() // �����̴����� ���� ���� CPU���� �Ѱܹ��� UAV���ۼ���.
{
	ID3D11Buffer* buffer;
	
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = mOutputStride * mOutputCount; // ������Ÿ��ũ�� * ����
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UAV�� ����.
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = mOutputStride;	// ������Ÿ��ũ��(����ũ��)

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer)); // buffer�� desc���� ���.

	mUAVbuffer = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::CreateUAV() // gpu���� ����� �����͸� �ٽ� ��Ƽ� cpu�� ������������ view / output���� uav�� ��ȯ.
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)mUAVbuffer;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = mOutputCount;

	V(DEVICE->CreateUnorderedAccessView(buffer, &uavDesc, &mUAV));
}

void ComputeStructuredBuffer::CreateResult() // ������ī���ϱ����� ���ۻ���.
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	((ID3D11Buffer*)mUAVbuffer)->GetDesc(&desc); // UAVbuffer ���� �״��. �翬�� ���ƾ��ϴϱ�

	desc.Usage = D3D11_USAGE_STAGING;	// CPU���� �б����� STAGING���� ����. 
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // �����̴����� �Ѿ�� ���� CPU�� ���� �� �ֵ���.

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer)); // desc���ÿ� ���� buffer ����.

	mReturnedDataFromGPU = (ID3D11Resource*)buffer;
}

void ComputeStructuredBuffer::Copy(void* data, UINT size) // �ƿ�ǲ���۱���ü, �޾ƿ;��� ��ü ũ�� (����üũ�� * ����) , ���������� �����ϴ� ����.
{
	// ��¹��۸� cpu�޸𸮿� ����.
	DEVICECONTEXT->CopyResource(mReturnedDataFromGPU, mUAVbuffer);  // CPU���� ���� �� �ִ� ���ۿ� ī��. CPU���� �Ⱦ��ٸ�(�׳� �� �ٷ� GPU�� �ѱ浥���Ͷ�ų� �׷���)  ���� ī�Ǿ����൵��.

	// �ڷḦ �����ؼ� �о���δ�.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	DEVICECONTEXT->Map(mReturnedDataFromGPU, 0, D3D11_MAP_READ, 0, &mappedData); // subresource�� ��ǻƮ���̴����� ���� �������޸��� �ּ� ���ͼ� ����.
	// mappedData�� ���ϵȰ��� ����ִ�.
	
	memcpy(data, mappedData.pData, size); // mappedData.pData���� size��ŭ�� ũ�⸦ data�� ����.
	DEVICECONTEXT->Unmap(mReturnedDataFromGPU, 0);
}