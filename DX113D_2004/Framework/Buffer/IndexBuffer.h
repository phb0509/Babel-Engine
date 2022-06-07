#pragma once

class IndexBuffer
{

public:
	IndexBuffer(void* data, UINT count);
	~IndexBuffer();

	void IASet();	

private:
	ID3D11Buffer* mBuffer;
};