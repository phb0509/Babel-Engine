#include "Framework.h"

VertexShader::VertexShader(wstring file, string entry)
{
	wstring path = L"Shaders/" + file + L".hlsl";

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V(D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), "vs_5_0",
		flags, 0, &mBlob, nullptr));

	V(DEVICE->CreateVertexShader(mBlob->GetBufferPointer(),
		mBlob->GetBufferSize(), nullptr, &mShader));

	CreateInputLayout();

	mBlob->Release();
}

VertexShader::~VertexShader()
{
	mShader->Release();
	mInputLayout->Release();	
}

void VertexShader::Set()
{
	DEVICECONTEXT->IASetInputLayout(mInputLayout);
	DEVICECONTEXT->VSSetShader(mShader, nullptr, 0);
}

void VertexShader::CreateInputLayout()
{
	D3DReflect(mBlob->GetBufferPointer(), mBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&mReflection);

	D3D11_SHADER_DESC shaderDesc;
	mReflection->GetDesc(&shaderDesc);

	vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts;

	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		mReflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask < 4)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask < 8)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask < 16)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string temp = paramDesc.SemanticName;
		if (temp == "Position")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		int n = temp.find_first_of('_');
		temp = temp.substr(0, n);

		if (temp == "Instance")
		{
			elementDesc.InputSlot = 1; // 레지스터번호인듯?
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;  // 입력원소는 인스턴스별 자료이다..를 명시.
			elementDesc.InstanceDataStepRate = 1; // 인스턴스별자료와 인스턴스를 1:1로 대응.
		}

		inputLayouts.emplace_back(elementDesc);
	}

	V(DEVICE->CreateInputLayout(inputLayouts.data(), (UINT)inputLayouts.size(),
		mBlob->GetBufferPointer(), mBlob->GetBufferSize(), &mInputLayout));
	
	mReflection->Release();
}
