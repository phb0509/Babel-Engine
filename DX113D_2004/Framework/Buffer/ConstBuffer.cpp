#include "Framework.h"

ConstBuffer::ConstBuffer(void* data, UINT dataSize)
	: data(data), dataSize(dataSize) // GPU�� �ѱ� ����ü(ex.mInputBuffer. ����������), ����üũ��(siaeof(Structure))
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	

	V(DEVICE->CreateBuffer(&desc, nullptr, &buffer));
}

ConstBuffer::~ConstBuffer()
{
	buffer->Release();
}

void ConstBuffer::Update()
{
	DEVICECONTEXT->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mMappedData);  // buffer�� ������ ����,subResource�� �����͸� �� ������.
	memcpy(mMappedData.pData, data, dataSize); // data(ex.mInputBuffer) -> subResource �� dataSize��ŭ �޸�ī��.
	DEVICECONTEXT->Unmap(buffer, 0);
}

void ConstBuffer::SetVSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->VSSetConstantBuffers(slot, 1, &buffer);
}

void ConstBuffer::SetPSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->PSSetConstantBuffers(slot, 1, &buffer);
}

void ConstBuffer::SetCSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->CSSetConstantBuffers(slot, 1, &buffer);
}

void ConstBuffer::SetHSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->HSSetConstantBuffers(slot, 1, &buffer);
}

void ConstBuffer::SetDSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->DSSetConstantBuffers(slot, 1, &buffer);
}

void ConstBuffer::SetGSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->GSSetConstantBuffers(slot, 1, &buffer);
}
