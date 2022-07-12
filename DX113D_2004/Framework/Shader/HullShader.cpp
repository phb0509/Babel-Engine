#include "Framework.h"

HullShader::HullShader(wstring file, string entry)
{
	wstring path = L"Shaders/" + file + L".hlsl";

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V(D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), "hs_5_0",
		flags, 0, &mBlob, nullptr));

	V(DEVICE->CreateHullShader(mBlob->GetBufferPointer(),
		mBlob->GetBufferSize(), nullptr, &mShader));

	mBlob->Release();
}

HullShader::~HullShader()
{
	mShader->Release();
}

void HullShader::Set()
{
	DEVICECONTEXT->HSSetShader(mShader, nullptr, 0);
}
