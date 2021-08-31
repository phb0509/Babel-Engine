#include "Framework.h"

VertexBuffer::VertexBuffer(void* data, UINT stride, UINT count, bool isCPUWrite) // 벡터주소, 원소크기, 벡터크기
	: stride(stride), offset(0)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	if (isCPUWrite)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	}
		
	vertexBufferDesc.ByteWidth = stride * count; // stride는 버텍스 하나의 크기
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = data;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	

	V(DEVICE->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &buffer));
}

VertexBuffer::~VertexBuffer()
{
	buffer->Release();
}

void VertexBuffer::IASet(UINT slot)
{
	DEVICECONTEXT->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}

void VertexBuffer::Update(void* data, UINT count) // vertices.data, vertices.size(원소개수)
{
	DEVICECONTEXT->UpdateSubresource(buffer, 0, nullptr, data, stride, count);  // 벡터주소,원소크기,원소개수 
	//deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
}

void VertexBuffer::Map(void* data, UINT dataSize) // subResource.pData, data, dataSize
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	mappedResource.pData = data;
	DEVICECONTEXT->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, dataSize);
	DEVICECONTEXT->Unmap(buffer, 0);
}

