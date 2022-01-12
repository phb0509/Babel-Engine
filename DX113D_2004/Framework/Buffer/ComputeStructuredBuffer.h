#pragma once

class ComputeStructuredBuffer
{
private:
	ID3D11Resource* input;
	ID3D11ShaderResourceView* srv;

	ID3D11Resource* mUAVbuffer;
	ID3D11UnorderedAccessView* mUAV;

	ID3D11Resource* mReturnedDataFromGPU;

	void* inputData;

	UINT inputStride;
	UINT inputCount;

	UINT outputStride;
	UINT outputCount;

public:
	ComputeStructuredBuffer(void* inputData, UINT inputStride, UINT inputCount,
		UINT outputStride = 0, UINT outputCount = 0);
	ComputeStructuredBuffer(UINT outputStride = 0, UINT outputCount = 0);
	~ComputeStructuredBuffer();

	void Copy(void* data, UINT size);

	ID3D11UnorderedAccessView*& GetUAV() { return mUAV; }
	ID3D11ShaderResourceView*& GetSRV() { return srv; }

private:
	void CreateInput();
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();
};