#pragma once

class Sphere : public Transform
{
private:
	typedef VertexUVNormalTangent VertexType;

	Mesh* mesh;
	Material* material;

	TypeBuffer* typeBuffer;

	vector<VertexType> vertices;
	vector<UINT> indices;

	UINT sliceCount;
	UINT stackCount;
	float radius;

public:
	Sphere(wstring shaderFile, float radius = 1.0f,
		UINT sliceCount = 20, UINT stackCount = 10);
	~Sphere();

	void Update();
	void Render();

	void CreateMesh();
	void CreateTangent();

	Material* GetMaterial() { return material; }
	float GetRadius() { return radius; }
};