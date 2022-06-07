#include "Framework.h"

VertexBuffer::VertexBuffer(void* data, UINT stride, UINT count, bool isCPUWrite) : // �����ּ�, ����ũ��, ����ũ��  
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
		
	vertexBufferDesc.ByteWidth = stride * count; // stride�� ���ؽ� �ϳ��� ũ��
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

void VertexBuffer::Update(void* data, UINT count) // vertices.data, vertices.size(���Ұ���)
{
	DEVICECONTEXT->UpdateSubresource(
		mBuffer, // GPU�� �ѱ� ����. ���⿡ �ѱ���� �ϴ� �����͸� �����Ŵ�.
		0,		// DstSubResource. MipLevel�̶� ǥ���ѻ��������.
		nullptr, // D3D11_BOX.
		data,    // �ѱ� �������� ù�ּ�. ����.data�� ��������.
		mStride,  //  ����ũ��// srcRowPitch , ����ü �� ���� ũ��.  // Texture���� width * sizeData. �� Width�� ũ��.
		count);  // ���Ұ���   // srcDepthPitch

	//deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
}

void VertexBuffer::Map(void* data, UINT dataSize) // vector.data, sizeof(Data) * DataCount
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	mappedResource.pData = data; // ���ؽ��� ���� ����.data (ù�ּ�)

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
		mBuffer, // �ѱ� ����
		0,		// DstSubResource. MipLevel�̶� ǥ���ѻ��������.
		&box, // D3D11_BOX.
		data,    // �ѱ� �������� ù�ּ�. ����.data�� ��������.
		0,  // width * sizeData , ����ũ��// srcRowPitch , ����ü �� ���� ũ��.
		0);  // ���Ұ���   // srcDepthPitch

}



