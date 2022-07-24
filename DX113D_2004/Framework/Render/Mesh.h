#pragma once

class Mesh
{
private:
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;

public:
	Mesh(void* vertexData, UINT stride, UINT vertexCount,
		void* indexData, UINT indexCount, bool isCPUWrite = false);
	~Mesh();

	void SetIA(D3D11_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void UpdateVertex(void* data, UINT count); // 벡터주소,원소개수
	void UpdateVertexUsingMap(void* data, UINT dataSize);
	void UpdateVertexUsingBox(void* data, UINT count, Vector3 pickedPosition, float range, float width, float height);

	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer->GetBuffer(); }
};