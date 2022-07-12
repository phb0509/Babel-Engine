#pragma once

class BinaryWriter
{

public:
	BinaryWriter(wstring filePath);
	BinaryWriter(string filePath);
	~BinaryWriter();

	void Int(int data);
	void UInt(UINT data);
	void Float(float data);
	void String(string data);
	void Float4x4(XMFLOAT4X4 data);
	void Byte(void* data, UINT dataSize);
	void CloseWriter();

private:
	HANDLE mFile;
	DWORD mSize;

};