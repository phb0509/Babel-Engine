#pragma once

class TerrainLOD : public Transform
{
private:
	typedef VertexUV VertexType;

	TerrainBuffer* buffer;

	Material* material;
	Mesh* mesh;

	HullShader* hullShader;
	DomainShader* domainShader;

	UINT cellsPerPatch;

	UINT width, height;
	UINT patchWidth, patchHeight;

	vector<VertexType> vertices;
	vector<UINT> indices;

	wstring heightFile;
	Texture* heightTexture;

	Frustum* frustum;
public:
	TerrainLOD(wstring heightFile);
	~TerrainLOD();

	void Update();
	void Render();
	void PostRender();

	float GetWidth();
	float GetHeight();
private:
	void ReadHeightData();
	void CreatePatchVertex();
	void CreatePatchIndex();	
};