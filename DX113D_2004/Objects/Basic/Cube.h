#pragma once

class Cube : public Transform
{
private:
	typedef VertexUVNormal VertexType;


public:
	Cube();	
	~Cube();

	void Update();
	void Render();
	void SetShader(wstring file);
	void SetMesh();
	Material* GetMaterial() { return mMaterial; }

private:
	void createMesh();
	void updateVertexNormal();


private:
	Material* mMaterial;
	Mesh* mMesh;

	vector<VertexType> mVertices;
	vector<UINT> mIndices;
};