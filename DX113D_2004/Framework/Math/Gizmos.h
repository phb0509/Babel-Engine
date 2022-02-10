#pragma once

class Gizmos
{
	struct GizmosHashColor
	{
		Float4 x;
		Float4 y;
		Float4 z;
	};

public:

	Gizmos();
	~Gizmos();

	void SetParent(Transform* parent);
private:




private:
	RasterizerState* mRSState;

	Mesh* mMesh;
	Material* mMaterial;

	MatrixBuffer* mWorldBuffer;
	vector<VertexColor> mVertices;
	vector<UINT> mIndices;

	ColorBuffer* mColorBuffer;
	Float4 mHashColorForBuffer;
	GizmosHashColor mHashColor;
};