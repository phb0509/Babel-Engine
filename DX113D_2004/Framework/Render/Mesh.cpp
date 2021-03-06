#include "Framework.h"

Mesh::Mesh(void* vertexData, UINT stride, UINT vertexCount, void* indexData, UINT indexCount, bool isCPUWrite)
             //vertices.data, 버텍스원소크기, 버텍스원소개수,  indices.data,   인덱스원소개수
{
	mVertexBuffer = new VertexBuffer(vertexData, stride, vertexCount, isCPUWrite);
	mIndexBuffer = new IndexBuffer(indexData, indexCount);
}

Mesh::~Mesh()
{
	GM->SafeDelete(mVertexBuffer);
	GM->SafeDelete(mIndexBuffer);
}

void Mesh::SetIA(D3D11_PRIMITIVE_TOPOLOGY primitiveType)
{
	mVertexBuffer->SetIA();
	mIndexBuffer->SetIA();
	DEVICECONTEXT->IASetPrimitiveTopology(primitiveType);
}

void Mesh::UpdateVertex(void* data, UINT count) // 벡터주소,원소개수
{
	mVertexBuffer->Update(data, count); // UpdateSubresource (DEFAULT) // 벡터.data, 벡터원소개수.
}

void Mesh::UpdateVertexUsingBox(void* data, UINT count, Vector3 pickedPosition, float range, float width, float height)
{
	mVertexBuffer->UpdateBox(data, count, pickedPosition, range, width, height); // UpdateSubresource (DEFAULT) // 벡터.data, 벡터원소개수.
}

void Mesh::UpdateVertexUsingMap(void* data, UINT dataSize)
{
	mVertexBuffer->Map(data, dataSize); // map , unmap  (DYNAMIC) // vector.data, sizeof(Data) * DataCount
}