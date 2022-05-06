#pragma once

class TerrainLOD : public Transform
{

public:
	TerrainLOD(wstring heightFile);
	~TerrainLOD();

	void Update();
	void Render();
	void PostRender();

	float GetTargetPositionY(Vector3 Target);

	void SetCamera(Camera* camera) {mCamera = camera;}

private:
	void readHeightData();
	void createPatchVertex();
	void createPatchIndex();	
	void createTempVertices();

private:
	typedef VertexUV VertexType;

	LODTerrainBuffer* mTerrainBuffer;

	Material* mMaterial;
	Mesh* mMesh;

	HullShader* mHullShader;
	DomainShader* mDomainShader;

	UINT mCellsPerPatch;
	UINT mTextureDefaultWidth;
	UINT mTextureDefaultHeight;
	UINT mPatchWidth;
	UINT mPatchHeight;
	float mFinalWidth;
	float mFinalHeight;

	vector<VertexType> mVertices;
	vector<VertexType> mTempVertices;
	vector<UINT> mIndices;

	wstring mHeightMapFileName;
	Texture* mHeightTexture;

	Camera* mCamera;
};