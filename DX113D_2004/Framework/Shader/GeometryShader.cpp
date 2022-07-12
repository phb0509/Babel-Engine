#include "Framework.h"

GeometryShader::GeometryShader(wstring file, string entry)
{
	wstring path = L"Shaders/" + file + L".hlsl";

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V(D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), "gs_5_0",
		flags, 0, &mBlob, nullptr));

	V(DEVICE->CreateGeometryShader(mBlob->GetBufferPointer(),
		mBlob->GetBufferSize(), nullptr, &mShader));

	mBlob->Release();
}

GeometryShader::~GeometryShader()
{
	mShader->Release();
}

void GeometryShader::Set()
{
	DEVICECONTEXT->GSSetShader(mShader, nullptr, 0);
}
