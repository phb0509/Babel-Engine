#pragma once

class ModelMesh
{
private:
	friend class ModelReader;

	ModelMesh();
	~ModelMesh();

	void CreateMesh();
	void Render();
	void Render(UINT drawCount);

	void SetBox(Vector3* minBox, Vector3* maxBox);
	void IASet();

private:
	string mName;
	string mMaterialName;

	Material* mMaterial;
	Mesh* mMesh;

	ModelVertex* mVertices;
	UINT* mIndices;

	UINT mVertexCount;
	UINT mIndexCount;
};