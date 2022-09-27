#pragma once

class Terrain : public Transform
{
private:
	typedef VertexUVNormalTangent VertexType;

	struct InputDesc
	{
		UINT index;
		Float3 v0, v1, v2;
	};

	struct OutputDesc
	{
		int picked;
		float u, v, distance;
	};


public:
	Terrain();
	Terrain(wstring hegihtMapName);
	~Terrain();

	void Update();	
	void Render();
	void DeferredRender();
	
	bool Picking(OUT Vector3* position);
	bool ComputePicking(OUT Vector3* position);

	Material* GetMaterial() { return mMaterial; }
	float GetTargetPositionY(Vector3 position);
	Float2 GetSize() { return Float2(mTerrainWidth, mTerrainHeight); }
	vector<Node*> GetNodeMap() { return mNodeMap; }

	void SetCamera(Camera* camera) { mCamera = camera; }
	
private:
	void createMesh();
	void createNormal();
	void createTangent();
	void createNodeMap();
	void createComputeData();

private:
	Material* mMaterial;
	Mesh* mMesh;
	vector<VertexType> mVertices;
	vector<UINT> mIndices;

	UINT mTerrainWidth;
	UINT mTerrainHeight;

	Texture* mHeightMap;
	RasterizerState* mFillMode[2];

	ComputeShader* mComputeShader;
	RayBuffer* mRayBuffer;
	ComputeStructuredBuffer* mStructuredBuffer;
	InputDesc* mInput;
	OutputDesc* mOutput;

	TypeBuffer* mTypeBuffer;
	UINT mPolygonCount;
	Camera* mCamera;
	vector<Node*> mNodeMap;
	Float2 mDistanceBetweenNodes;
	Int2 mNodeCount;
};