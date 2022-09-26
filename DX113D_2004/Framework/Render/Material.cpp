#include "Framework.h"

Material::Material(): 
	mVertexShader(nullptr), 
	mPixelShader(nullptr),
	mDiffuseMap(nullptr), 
	mSpecularMap(nullptr), 
	mNormalMap(nullptr), 
	mBrushMap(nullptr)
{
	mBuffer = new MaterialBuffer();
}

Material::Material(wstring file): 
	mDiffuseMap(nullptr),
	mSpecularMap(nullptr), 
	mNormalMap(nullptr),
	mBrushMap(nullptr)
{
	mVertexShader = Shader::AddVS(file);
	mPixelShader = Shader::AddPS(file);

	mBuffer = new MaterialBuffer();
}

Material::Material(VertexShader* vertexShader, PixelShader* pixelShader): 
	mVertexShader(vertexShader), 
	mPixelShader(pixelShader),
	mDiffuseMap(nullptr), 
	mSpecularMap(nullptr), 
	mNormalMap(nullptr),
	mBrushMap(nullptr)
{
	mBuffer = new MaterialBuffer();
}

Material::~Material()
{
	GM->SafeDelete(mBuffer);
}

void Material::Set()
{
	if (mDiffuseMap != nullptr)
		mDiffuseMap->PSSet(0);

	if (mSpecularMap != nullptr)
		mSpecularMap->PSSet(1);

	if (mNormalMap != nullptr)
		mNormalMap->PSSet(2);

	if (mBrushMap != nullptr)
		mBrushMap->PSSet(7);

	mBuffer->SetPSBuffer(1);

	mVertexShader->Set();
	mPixelShader->Set();
}

void Material::SetShader(wstring file)
{
	mVertexShader = Shader::AddVS(file);
	mPixelShader = Shader::AddPS(file);
}

void Material::SetDiffuseMap(wstring file)
{
	mDiffuseMap = Texture::Add(file);
	mBuffer->data.hasDiffuseMap = 1;
}

void Material::SetSpecularMap(wstring file)
{
	mSpecularMap = Texture::Add(file);
	mBuffer->data.hasSpecularMap = 1;
}

void Material::SetNormalMap(wstring file)
{
	mNormalMap = Texture::Add(file);
	mBuffer->data.hasNormalMap = 1;
}

void Material::SetBrushMap(wstring file)
{
	mBrushMap = Texture::Add(file);
}

void Material::SetDiffuseMap(Texture* diffuseMap)
{
	mDiffuseMap = diffuseMap;
	mBuffer->data.hasDiffuseMap = 1;
}

void Material::SetSpecularMap(Texture* specularMap)
{
	mSpecularMap = specularMap;
	mBuffer->data.hasSpecularMap = 1;
}

void Material::SetNormalMap(Texture* normalMap)
{
	mNormalMap = normalMap;
	mBuffer->data.hasNormalMap = 1;
}

void Material::SetBrushMap(Texture* brushMap)
{
	mBrushMap = brushMap;
}

void Material::SetHasSpecularMap(bool value)
{
	mBuffer->data.hasSpecularMap = value;
}
