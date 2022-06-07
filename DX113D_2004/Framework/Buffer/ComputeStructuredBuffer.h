#pragma once

class ComputeStructuredBuffer
{

public:
	ComputeStructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride = 0, UINT outputCount = 0);
	ComputeStructuredBuffer(UINT outputStride = 0, UINT outputCount = 0);
	~ComputeStructuredBuffer();

	void Copy(void* data, UINT size);

	ID3D11UnorderedAccessView*& GetUAV() { return mUAV; }
	ID3D11ShaderResourceView*& GetSRV() { return mSRV; }

private:
	void CreateInput();
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();

private:
	ID3D11Resource* mInput;
	ID3D11ShaderResourceView* mSRV;

	ID3D11Resource* mUAVbuffer;
	ID3D11UnorderedAccessView* mUAV;

	ID3D11Resource* mReturnedDataFromGPU;

	void* mInputData;

	UINT mInputStride;
	UINT mInputCount;

	UINT mOutputStride;
	UINT mOutputCount;
};