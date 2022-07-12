#pragma once

class HullShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	HullShader(wstring file, string entry);
	~HullShader();

private:
	ID3D11HullShader* mShader;

};