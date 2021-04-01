#pragma once

class ModelMesh
{
private:
	friend class ModelReader;

	string name;
	string materialName;

	Material* material;
	Mesh* mesh;

	ModelVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	ModelMesh();
	~ModelMesh();

	void CreateMesh();
	void Render();
	void Render(UINT drawCount);

	void SetBox(Vector3* minBox, Vector3* maxBox);
};