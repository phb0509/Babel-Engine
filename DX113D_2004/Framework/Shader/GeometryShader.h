#pragma once

class GeometryShader : public Shader
{

public:
	virtual void Set() override;

private:
	friend class Shader;

	GeometryShader(wstring file, string entry);
	~GeometryShader();

private:
	ID3D11GeometryShader* mShader;

};