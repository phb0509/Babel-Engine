#include "Framework.h"

ConstBuffer::ConstBuffer(void* data, UINT dataSize) : 
	mData(data), 
	mDataSize(dataSize) // GPU로 넘길 구조체(ex.mInputBuffer. 실제데이터), 구조체크기(siaeof(Structure))
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	

	V(DEVICE->CreateBuffer(&desc, nullptr, &mBuffer));
}

ConstBuffer::~ConstBuffer()
{
	mBuffer->Release();
}

void ConstBuffer::Update()
{
	DEVICECONTEXT->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mMappedData);  // buffer는 매핑할 버퍼,subResource는 데이터를 쓸 포인터.
	memcpy(mMappedData.pData, mData, mDataSize); // data(ex.mInputBuffer) -> subResource 로 dataSize만큼 메모리카피.
	DEVICECONTEXT->Unmap(mBuffer, 0);
}

void ConstBuffer::SetVSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->VSSetConstantBuffers(slot, 1, &mBuffer);
}

void ConstBuffer::SetPSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->PSSetConstantBuffers(slot, 1, &mBuffer);
}

void ConstBuffer::SetCSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->CSSetConstantBuffers(slot, 1, &mBuffer);
}

void ConstBuffer::SetHSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->HSSetConstantBuffers(slot, 1, &mBuffer);
}

void ConstBuffer::SetDSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->DSSetConstantBuffers(slot, 1, &mBuffer);
}

void ConstBuffer::SetGSBuffer(UINT slot)
{
	Update();
	DEVICECONTEXT->GSSetConstantBuffers(slot, 1, &mBuffer);
}
