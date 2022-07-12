#pragma once

class VertexShader;
class PixelShader;
class ComputeShader;
class HullShader;
class DomainShader;
class GeometryShader;

class Shader
{
public:
	virtual void Set() = 0;

	static VertexShader* AddVS(wstring file, string entry = "VS");
	static PixelShader* AddPS(wstring file, string entry = "PS");
	static ComputeShader* AddCS(wstring file, string entry = "CS");
	static HullShader* AddHS(wstring file, string entry = "HS");
	static DomainShader* AddDS(wstring file, string entry = "DS");
	static GeometryShader* AddGS(wstring file, string entry = "GS");

	static void Delete();

protected:
	ID3DBlob* mBlob;

private:
	static map<wstring, Shader*> mTotalShader;

};