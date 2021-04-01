#include "Framework.h"

ModelMesh::ModelMesh()
{
}

ModelMesh::~ModelMesh()
{
	delete mesh;

	delete[] vertices;
	delete[] indices;
}

void ModelMesh::CreateMesh()
{
	mesh = new Mesh(vertices, sizeof(ModelVertex), vertexCount,
		indices, indexCount);
}

void ModelMesh::Render()
{
	mesh->IASet();
	material->Set();

	DC->DrawIndexed(indexCount, 0, 0);
}

void ModelMesh::Render(UINT drawCount)
{
	mesh->IASet();
	material->Set();

	DC->DrawIndexedInstanced(indexCount, drawCount, 0, 0, 0);
}

void ModelMesh::SetBox(Vector3* minBox, Vector3* maxBox)
{
	Float3 minPos = vertices[0].position;
	Float3 maxPos = vertices[0].position;

	for (UINT i = 1; i < vertexCount; i++)
	{
		minPos.x = min(minPos.x, vertices[i].position.x);
		minPos.y = min(minPos.y, vertices[i].position.y);
		minPos.z = min(minPos.z, vertices[i].position.z);

		maxPos.x = max(maxPos.x, vertices[i].position.x);
		maxPos.y = max(maxPos.y, vertices[i].position.y);
		maxPos.z = max(maxPos.z, vertices[i].position.z);
	}

	*minBox = minPos;
	*maxBox = maxPos;
}