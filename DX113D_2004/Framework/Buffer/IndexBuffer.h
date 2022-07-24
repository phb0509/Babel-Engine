#pragma once

class IndexBuffer
{

public:
	IndexBuffer(void* data, UINT count);
	~IndexBuffer();

	void SetIA();	

private:
	ID3D11Buffer* mBuffer;
};