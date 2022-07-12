#pragma once

class VertexShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	VertexShader(wstring file, string entry);
	~VertexShader();

private:
	void CreateInputLayout();

private:
	ID3D11VertexShader* mShader;
	ID3D11InputLayout* mInputLayout;
	ID3D11ShaderReflection* mReflection;

};