#pragma once

class TerrainLOD : public Transform
{

public:
	TerrainLOD(wstring heightFile);
	~TerrainLOD();

	void Update();
	void Render();
	void PostRender();

	float GetWidth();
	float GetHeight();
	void SetCamera(Camera* camera) {mCamera = camera;}

private:
	void readHeightData();
	void createPatchVertex();
	void createPatchIndex();	

private:
	typedef VertexUV VertexType;

	TerrainBuffer* mTerrainBuffer;

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

	Camera* mCamera;
};