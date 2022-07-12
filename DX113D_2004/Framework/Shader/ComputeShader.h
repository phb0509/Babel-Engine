#pragma once

class ComputeShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	ComputeShader(wstring file, string entry);
	~ComputeShader();

private:
	ID3D11ComputeShader* mShader;

};