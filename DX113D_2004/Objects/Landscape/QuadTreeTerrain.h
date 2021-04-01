#pragma once

class QuadTreeTerrain : public Transform
{
private:
	typedef VertexUVNormal VertexType;
	const UINT MIN_TRIANGLE = 10240;

	struct Node
	{
		float x, z, width;
		UINT triangleCount;

		Mesh* mesh;

		Node* children[4];
	};

	UINT triangleCount, drawCount;

	VertexType* vertices;

	Node* root;
	Frustum* frustum;

	UINT width, height;

	Material* material;

public:
	QuadTreeTerrain(TerrainData* terrainData);
	~QuadTreeTerrain();

	void Update();
	void Render();
	void PostRender();

private:
	void RenderNode(Node* node);
	void DeleteNode(Node* node);

	void CalcMeshDimensions(UINT vertexCount,
		float& centerX, float& centerZ, float& width);
	void CreateTreeNode(Node* node, float x, float z, float width);

	UINT ContainTriangleCount(float x, float z, float width);
	bool IsTriangleContained(UINT index, float x, float z, float width);
};