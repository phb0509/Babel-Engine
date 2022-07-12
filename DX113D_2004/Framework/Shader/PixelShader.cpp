#include "Framework.h"

PixelShader::PixelShader(wstring file, string entry)
{
	wstring path = L"Shaders/" + file + L".hlsl";

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V(D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), "ps_5_0",
		flags, 0, &mBlob, nullptr));

	V(DEVICE->CreatePixelShader(mBlob->GetBufferPointer(),
		mBlob->GetBufferSize(), nullptr, &mShader));

	mBlob->Release();
}

PixelShader::~PixelShader()
{
	mShader->Release();
}

void PixelShader::Set()
{
	DEVICECONTEXT->PSSetShader(mShader, nullptr, 0);
}
