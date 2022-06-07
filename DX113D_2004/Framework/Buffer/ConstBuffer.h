#pragma once

class ConstBuffer
{

protected:
	ConstBuffer(void* data, UINT dataSize);
	virtual ~ConstBuffer();

	void Update();

public:
	void SetVSBuffer(UINT slot);
	void SetPSBuffer(UINT slot);
	void SetCSBuffer(UINT slot);
	void SetHSBuffer(UINT slot);
	void SetDSBuffer(UINT slot);
	void SetGSBuffer(UINT slot);


private:
	ID3D11Buffer* mBuffer;

	void* mData;
	UINT mDataSize;

	D3D11_MAPPED_SUBRESOURCE mMappedData;
};