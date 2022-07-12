#pragma once

class PixelShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	PixelShader(wstring file, string entry);
	~PixelShader();

private:
	ID3D11PixelShader* mShader;

};