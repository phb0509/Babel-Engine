#pragma once

class Material
{
private:
	class MaterialBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float4 diffuse; // 난반사
			Float4 specular; // 정반사
			Float4 ambient; // 간접광
			Float4 emissive;

			float shininess; // 정반사할때 집중도?날카로움.

			int hasDiffuseMap;
			int hasSpecularMap;
			int hasNormalMap;
		}data;

		MaterialBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.diffuse = { 1, 1, 1, 1 }; //기본적으로 1,1,1,1이면 모든 색을 반사. 만약 1,0,0,1이면 빨간색만 반사
			data.specular = { 1, 1, 1, 1 };
			data.ambient = { 1, 1, 1, 1 };
			data.emissive = { 0.3f, 0.3f, 0.3f, 0.1f };
			data.shininess = 30;

			data.hasDiffuseMap = 0;
			data.hasSpecularMap = 0;
			data.hasNormalMap = 0;
		}
	};	


	
public:
	Material();
	Material(wstring file);
	Material(VertexShader* vertexShader, PixelShader* pixelShader);
	~Material();

	void Set();

	void SetShader(wstring file);

	void SetDiffuseMap(wstring file);
	void SetSpecularMap(wstring file);
	void SetNormalMap(wstring file);
	void SetBrushMap(wstring file);

	void SetDiffuseMap(Texture* diffuseMap);
	void SetSpecularMap(Texture* specularMap);
	void SetNormalMap(Texture* normalMap);
	void SetBrushMap(Texture* brushMap);
	MaterialBuffer* GetBuffer() { return mBuffer; }

public:
	string mName;

private:
	VertexShader* mVertexShader;
	PixelShader* mPixelShader;

	MaterialBuffer* mBuffer;

	Texture* mDiffuseMap;
	Texture* mSpecularMap;
	Texture* mNormalMap;
	Texture* mBrushMap;

};