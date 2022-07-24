#include "Framework.h"

Mesh::Mesh(void* vertexData, UINT stride, UINT vertexCount, void* indexData, UINT indexCount, bool isCPUWrite)
             //vertices.data, ���ؽ�����ũ��, ���ؽ����Ұ���,  indices.data,   �ε������Ұ���
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

void Mesh::UpdateVertex(void* data, UINT count) // �����ּ�,���Ұ���
{
	mVertexBuffer->Update(data, count); // UpdateSubresource (DEFAULT) // ����.data, ���Ϳ��Ұ���.
}

void Mesh::UpdateVertexUsingBox(void* data, UINT count, Vector3 pickedPosition, float range, float width, float height)
{
	mVertexBuffer->UpdateBox(data, count, pickedPosition, range, width, height); // UpdateSubresource (DEFAULT) // ����.data, ���Ϳ��Ұ���.
}

void Mesh::UpdateVertexUsingMap(void* data, UINT dataSize)
{
	mVertexBuffer->Map(data, dataSize); // map , unmap  (DYNAMIC) // vector.data, sizeof(Data) * DataCount
}