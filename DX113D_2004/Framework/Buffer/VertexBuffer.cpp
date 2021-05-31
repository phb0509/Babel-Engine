#include "Framework.h"

VertexBuffer::VertexBuffer(void* data, UINT stride, UINT count, bool isCPUWrite)
	: stride(stride), offset(0)
{
	D3D11_BUFFER_DESC desc = {};
	if (isCPUWrite)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}else
		desc.Usage = D3D11_USAGE_DEFAULT;

	desc.ByteWidth = stride * count; // stride는 버텍스 하나의 크기
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;

	V(DEVICE->CreateBuffer(&desc, &initData, &buffer));
}

VertexBuffer::~VertexBuffer()
{
	buffer->Release();
}

void VertexBuffer::IASet(UINT slot)
{
	DC->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}

void VertexBuffer::Update(void* data, UINT count)
{
	DC->UpdateSubresource(buffer, 0, nullptr, data, stride, count); 
}

void VertexBuffer::Map(void* data, UINT start, UINT size, UINT offset)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	subResource.pData = data;

	DC->Map(buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
	memcpy((BYTE*)subResource.pData + offset, (BYTE*)data + start, size);
	DC->Unmap(buffer, 0);
}

