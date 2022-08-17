#pragma once

class BinaryWriter
{

public:
	BinaryWriter(wstring filePath);
	BinaryWriter(string filePath);
	~BinaryWriter();

	void WriteInt(int data);
	void WriteUInt(UINT data);
	void WriteFloat(float data);
	void WriteString(string data);
	void WriteFloat4x4(XMFLOAT4X4 data);
	void Byte(void* data, UINT dataSize);
	void CloseWriter();

private:
	HANDLE mFile;
	DWORD mSize;
};