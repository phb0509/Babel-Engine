#pragma once

class Gizmos : public Transform
{
	struct HashColor
	{
		Float4 x;
		Float4 y;
		Float4 z;
	};

public:

	Gizmos();
	~Gizmos();

	void Update();
	void PreRender();
	void Render();
	 
private:
	void createMesh();
	void createGizmosHashColor();
	void createMeshForColorPicking();
	
private:
	RasterizerState* mRSState;

	Mesh* mMesh;
	Material* mMaterial;

	vector<VertexColor> mVertices;
	vector<UINT> mIndices;

	//..GizmosHashColor mHashColor;
	ColorBuffer* mHashColorBuffer;
	HashColor mHashColor;

};