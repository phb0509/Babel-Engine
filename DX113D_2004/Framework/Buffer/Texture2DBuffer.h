#pragma once


class Texture2DBuffer
{
public:
	Texture2DBuffer(ID3D11Texture2D* src);
	~Texture2DBuffer();

	UINT GetWidth() { return mWidth; }
	UINT GetHeight() {return mHeight; }
	UINT GetPage() { return mPage; }

	ID3D11Texture2D* GetInput() { return (ID3D11Texture2D*)mInput; }
	ID3D11Texture2D* GetOutput() { return (ID3D11Texture2D*)mOutput; }

	ID3D11ShaderResourceView*& GetSRV() { return mSRV; }
	ID3D11UnorderedAccessView*& GetUAV() { return mUAV; }
	ID3D11ShaderResourceView*& OutputSRV() { return mOutputSRV; }

private:
	void createSRV();
	void createOutput();
	void createUAV();
	void createOutputSRV();

private:
	// Shader�� ���� ���������� ����
	ID3D11Resource* mInput = nullptr;
	ID3D11ShaderResourceView* mSRV = nullptr;

	// Shader���� �� �޾ƿ������� ����.
	ID3D11Resource* mOutput = nullptr;
	ID3D11UnorderedAccessView* mUAV = nullptr;
	
	ID3D11Resource* mResult = nullptr;

	UINT mWidth;
	UINT mHeight;
	UINT mPage;
	DXGI_FORMAT mFormat;

	ID3D11ShaderResourceView* mOutputSRV;
};