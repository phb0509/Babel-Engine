#include "Framework.h"

ModelMesh::ModelMesh()
{
}

ModelMesh::~ModelMesh()
{
	delete mMesh;

	delete[] mVertices;
	delete[] mIndices;
}

void ModelMesh::CreateMesh()
{
	mMesh = new Mesh(mVertices, sizeof(ModelVertex), mVertexCount,
		mIndices, mIndexCount);
}

void ModelMesh::Render()
{
	mMesh->IASet();
	mMaterial->Set();

	DEVICECONTEXT->DrawIndexed(mIndexCount, 0, 0);
}

void ModelMesh::Render(UINT drawCount)
{
	mMesh->IASet();
	mMaterial->Set();

	DEVICECONTEXT->DrawIndexedInstanced(mIndexCount, drawCount, 0, 0, 0);

	mIndexCount;
	int a = 0; 
}

void ModelMesh::SetBox(Vector3* minBox, Vector3* maxBox)
{
	Float3 minPos = mVertices[0].position;
	Float3 maxPos = mVertices[0].position;

	for (UINT i = 1; i < mVertexCount; i++)
	{
		minPos.x = min(minPos.x, mVertices[i].position.x);
		minPos.y = min(minPos.y, mVertices[i].position.y);
		minPos.z = min(minPos.z, mVertices[i].position.z);

		maxPos.x = max(maxPos.x, mVertices[i].position.x);
		maxPos.y = max(maxPos.y, mVertices[i].position.y);
		maxPos.z = max(maxPos.z, mVertices[i].position.z);
	}

	*minBox = minPos;
	*maxBox = maxPos;
}

void ModelMesh::IASet()
{
	mMesh->IASet();
}
