#pragma once

class BinaryReader
{

public:
	BinaryReader(wstring filePath);
	BinaryReader(string filePath);
	~BinaryReader();

	int Int();
	UINT UInt();
	float Float();
	string String();
	XMFLOAT4X4 Float4x4();
	void Byte(void** data, UINT dataSize);
	void CloseReader();

private:
	HANDLE mFile;
	DWORD mSize;

};