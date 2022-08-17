#include "Framework.h"

BinaryWriter::BinaryWriter(wstring filePath) : 
	mSize(0)
{
	mFile = CreateFile(
		filePath.c_str(), 
		GENERIC_WRITE,
		0,
		0, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (mFile == INVALID_HANDLE_VALUE)
	{
		char buff[100];
		sprintf_s(buff, "File 1 Open Error (%d) : %s\n", GetLastError(), filePath.c_str());
		OutputDebugStringA(buff);

		CloseHandle(mFile);
	}
	else
	{
		char buff[100];
		sprintf_s(buff, "File 1 Open Error (%d) : %s\n", GetLastError(), filePath.c_str());
		OutputDebugStringA(buff);

		int b = 0;
	}
}

BinaryWriter::BinaryWriter(string filePath): 
	mSize(0)
{
	mFile = CreateFileA(
		filePath.c_str(),
		GENERIC_WRITE,
		0,
		0, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		nullptr);
}

BinaryWriter::~BinaryWriter()
{
	//CloseHandle(file);
}

void BinaryWriter::WriteInt(int data)
{
	WriteFile(mFile, &data, sizeof(int), &mSize, nullptr);
}

void BinaryWriter::WriteUInt(UINT data)
{
	WriteFile(mFile, &data, sizeof(UINT), &mSize, nullptr);
}

void BinaryWriter::WriteFloat(float data)
{
	WriteFile(mFile, &data, sizeof(float), &mSize, nullptr);
}

void BinaryWriter::WriteString(string data)
{
	WriteUInt((UINT)data.size());

	const char* str = data.c_str();
	WriteFile(mFile, str, sizeof(char) * (UINT)data.size(), &mSize, nullptr);
}

void BinaryWriter::WriteFloat4x4(XMFLOAT4X4 data)
{
	WriteFile(mFile, &data, sizeof(XMFLOAT4X4), &mSize, nullptr);
}

void BinaryWriter::Byte(void* data, UINT dataSize)
{
	WriteFile(mFile, data, dataSize, &mSize, nullptr);
}

void BinaryWriter::CloseWriter()
{
	CloseHandle(mFile);
}
