#include "Framework.h"

BinaryWriter::BinaryWriter(wstring filePath) : 
	mSize(0)
{
	mFile = CreateFile(filePath.c_str(), GENERIC_WRITE,
		0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

BinaryWriter::BinaryWriter(string filePath): 
	mSize(0)
{
	mFile = CreateFileA(filePath.c_str(), GENERIC_WRITE,
		0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

BinaryWriter::~BinaryWriter()
{
	//CloseHandle(file);
}

void BinaryWriter::Int(int data)
{
	WriteFile(mFile, &data, sizeof(int), &mSize, nullptr);
}

void BinaryWriter::UInt(UINT data)
{
	WriteFile(mFile, &data, sizeof(UINT), &mSize, nullptr);
}

void BinaryWriter::Float(float data)
{
	WriteFile(mFile, &data, sizeof(float), &mSize, nullptr);
}

void BinaryWriter::String(string data)
{
	UInt((UINT)data.size());

	const char* str = data.c_str();
	WriteFile(mFile, str, sizeof(char) * (UINT)data.size(), &mSize, nullptr);
}

void BinaryWriter::Float4x4(XMFLOAT4X4 data)
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
