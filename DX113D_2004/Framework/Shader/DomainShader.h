#pragma once

class DomainShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	DomainShader(wstring file, string entry);
	~DomainShader();

private:
	ID3D11DomainShader* mShader;

};