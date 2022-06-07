#include "Framework.h"

VertexBuffer::VertexBuffer(void* data, UINT stride, UINT count, bool isCPUWrite) : // 벡터주소, 원소크기, 벡터크기  
	mStride(stride), 
	mOffset(0)
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
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = data;
	//vertexBufferData.SysMemPitch = 0;
	//vertexBufferData.SysMemSlicePitch = 0;
	

	V(DEVICE->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &mBuffer));
}

VertexBuffer::~VertexBuffer()
{
	mBuffer->Release();
}

void VertexBuffer::IASet(UINT slot)
{
	DEVICECONTEXT->IASetVertexBuffers(slot, 1, &mBuffer, &mStride, &mOffset);
}

void VertexBuffer::Update(void* data, UINT count) // vertices.data, vertices.size(원소개수)
{
	DEVICECONTEXT->UpdateSubresource(
		mBuffer, // GPU에 넘길 버퍼. 여기에 넘기고자 하는 데이터를 담을거다.
		0,		// DstSubResource. MipLevel이라 표기한사람도있음.
		nullptr, // D3D11_BOX.
		data,    // 넘길 데이터의 첫주소. 벡터.data로 쓰고있음.
		mStride,  //  원소크기// srcRowPitch , 구조체 한 개의 크기.  // Texture에선 width * sizeData. 즉 Width의 크기.
		count);  // 원소개수   // srcDepthPitch

	//deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
}

void VertexBuffer::Map(void* data, UINT dataSize) // vector.data, sizeof(Data) * DataCount
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	mappedResource.pData = data; // 버텍스들 담은 벡터.data (첫주소)

	DEVICECONTEXT->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, dataSize);
	DEVICECONTEXT->Unmap(mBuffer, 0);
}

void VertexBuffer::UpdateBox(void* data, UINT count, Vector3 pickedPosition, float range, float width, float height)
{
	//D3D11_BOX box;
	//box.left = width * (pickedPosition.y - range) + (pickedPosition.x - range);
	//box.right = width * (pickedPosition.y + range) + (pickedPosition.y + range);
	//box.top = 0;
	//box.bottom = 1;
	//box.front = 0;
	//box.back = 1;

	//if (box.left <= 0)
	//{
	//	box.left = 0;
	//}

	//if (box.right >= count)
	//{
	//	box.right = count - 1;
	//}

	D3D11_BOX box;
	box.left = width * mStride;
	box.right = height * mStride;
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;
	
	DEVICECONTEXT->UpdateSubresource(
		mBuffer, // 넘길 버퍼
		0,		// DstSubResource. MipLevel이라 표기한사람도있음.
		&box, // D3D11_BOX.
		data,    // 넘길 데이터의 첫주소. 벡터.data로 쓰고있음.
		0,  // width * sizeData , 원소크기// srcRowPitch , 구조체 한 개의 크기.
		0);  // 원소개수   // srcDepthPitch

}



