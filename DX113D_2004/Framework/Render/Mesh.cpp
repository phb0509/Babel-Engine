#include "Framework.h"

Mesh::Mesh(void* vertexData, UINT stride, UINT vertexCount, void* indexData, UINT indexCount, bool isCPUWrite)
{
	vertexBuffer = new VertexBuffer(vertexData, stride, vertexCount, isCPUWrite);
	indexBuffer = new IndexBuffer(indexData, indexCount);
}

Mesh::~Mesh()
{
	delete vertexBuffer;
	delete indexBuffer;
}

void Mesh::IASet(D3D11_PRIMITIVE_TOPOLOGY primitiveType)
{
	vertexBuffer->IASet();
	indexBuffer->IASet();
	DEVICECONTEXT->IASetPrimitiveTopology(primitiveType);
}

void Mesh::UpdateVertex(void* data, UINT count)
{
	vertexBuffer->Update(data, count); // UpdateSubresource (DEFAULT)
}

void Mesh::UpdateVertexUsingMap(void* data, UINT dataSize)
{
	vertexBuffer->Map(data, dataSize); // map , unmap  (DYNAMIC)
}
