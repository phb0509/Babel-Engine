#pragma once

class BinaryReader
{

public:
	BinaryReader(wstring filePath);
	BinaryReader(wstring filePath, OUT bool& isSuccessedLoadFile);
	BinaryReader(string filePath, OUT bool& isSuccessedLoadFile);
	BinaryReader(string filePath);
	~BinaryReader();

	int ReadInt();
	UINT ReadUInt();
	float ReadFloat();
	string ReadString();
	XMFLOAT4X4 ReadFloat4x4(); // Matrix
	void Byte(void** data, UINT dataSize);
	void CloseReader();
	bool CheckSameFile(wstring filePath);

private:
	HANDLE mFile;
	DWORD mSize;

};