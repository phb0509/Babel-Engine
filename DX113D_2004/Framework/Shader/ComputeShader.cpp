#include "Framework.h"

ComputeShader::ComputeShader(wstring file, string entry)
{
	wstring path = L"Shaders/" + file + L".hlsl";

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V(D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), "cs_5_0",
		flags, 0, &blob, nullptr));

	V(DEVICE->CreateComputeShader(blob->GetBufferPointer(),
		blob->GetBufferSize(), nullptr, &shader));

	blob->Release();
}

ComputeShader::~ComputeShader()
{
	shader->Release();
}

void ComputeShader::Set()
{
	DEVICECONTEXT->CSSetShader(shader, nullptr, 0);
}
