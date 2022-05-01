#pragma once

class Material
{
private:
	class MaterialBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float4 diffuse; // ���ݻ�
			Float4 specular; // ���ݻ�
			Float4 ambient; // ������
			Float4 emissive;

			float shininess; // ���ݻ��Ҷ� ���ߵ�?��ī�ο�.

			int hasDiffuseMap;
			int hasSpecularMap;
			int hasNormalMap;
		}data;

		MaterialBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; //�⺻������ 1,1,1,1�̸� ��� ���� �ݻ�. ���� 1,0,0,1�̸� �������� �ݻ�
			data.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
			data.ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
			data.emissive = { 0.0f, 0.0f, 0.0f, 1.0f };
			data.shininess = 24.0f;

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

	void SetHasSpecularMap(bool value);
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