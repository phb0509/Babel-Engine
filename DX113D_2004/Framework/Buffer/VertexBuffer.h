#pragma once

class VertexBuffer
{
public:
	VertexBuffer(void* data, UINT stride, UINT count, bool isCPUWrite = false);	
	~VertexBuffer();

	void SetIA(UINT slot = 0);

	void Update(void* data, UINT count);
	void Map(void* data,  UINT dataSize);
	void UpdateBox(void* data, UINT count, Vector3 pickedPosition, float range, float width, float height);
	
	ID3D11Buffer* GetBuffer() { return mBuffer; }

private:
	ID3D11Buffer* mBuffer;

	UINT mStride;
	UINT mOffset;
};