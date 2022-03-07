#include "Framework.h"

Cube::Cube()
{
	mMaterial = new Material(L"Diffuse");
	mMaterial->SetDiffuseMap(L"Textures/test.jpg");

	createMesh();
}

Cube::~Cube()
{
	delete mMaterial;
	delete mMesh;

}

void Cube::Update()
{	
	UpdateWorld();
}

void Cube::Render()
{
	mMesh->IASet();

	SetWorldBuffer();

	mMaterial->Set();

	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

void Cube::SetShader(wstring file)
{
	mMaterial->SetShader(file);
}

void Cube::SetMesh()
{
	mMesh->IASet();

	mMaterial->Set();

	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

void Cube::createMesh()
{
	mVertices.resize(24);
	//Front
	mVertices[0].position = { -1, -1, -1 };
	mVertices[1].position = { -1, +1, -1 };
	mVertices[2].position = { +1, +1, -1 };
	mVertices[3].position = { +1, -1, -1 };

	//Back
	mVertices[4].position = { -1, -1, +1 };
	mVertices[5].position = { -1, +1, +1 };
	mVertices[6].position = { +1, +1, +1 };
	mVertices[7].position = { +1, -1, +1 };

	//Right
	mVertices[8].position = { +1, -1, -1 };
	mVertices[9].position = { +1, +1, -1 };
	mVertices[10].position = { +1, +1, +1 };
	mVertices[11].position = { +1, -1, +1 };

	//Left
	mVertices[12].position = { -1, -1, -1 };
	mVertices[13].position = { -1, +1, -1 };
	mVertices[14].position = { -1, +1, +1 };
	mVertices[15].position = { -1, -1, +1 };

	//Up
	mVertices[16].position = { -1, +1, -1 };
	mVertices[17].position = { -1, +1, +1 };
	mVertices[18].position = { +1, +1, +1 };
	mVertices[19].position = { +1, +1, -1 };

	//Down
	mVertices[20].position = { -1, -1, -1 };
	mVertices[21].position = { -1, -1, +1 };
	mVertices[22].position = { +1, -1, +1 };
	mVertices[23].position = { +1, -1, -1 };

	for (UINT i = 0; i < 6; i++)
	{
		mVertices[i * 4 + 0].uv = { 0, 1 };
		mVertices[i * 4 + 1].uv = { 0, 0 };
		mVertices[i * 4 + 2].uv = { 1, 0 };
		mVertices[i * 4 + 3].uv = { 1, 1 };
	}	

	//Front
	mIndices.emplace_back(0);
	mIndices.emplace_back(1);
	mIndices.emplace_back(2);

	mIndices.emplace_back(0);
	mIndices.emplace_back(2);
	mIndices.emplace_back(3);

	//Back
	mIndices.emplace_back(4);
	mIndices.emplace_back(6);
	mIndices.emplace_back(5);

	mIndices.emplace_back(4);
	mIndices.emplace_back(7);
	mIndices.emplace_back(6);

	//Right
	mIndices.emplace_back(8);
	mIndices.emplace_back(9);
	mIndices.emplace_back(10);

	mIndices.emplace_back(8);
	mIndices.emplace_back(10);
	mIndices.emplace_back(11);

	//Left
	mIndices.emplace_back(12);
	mIndices.emplace_back(14);
	mIndices.emplace_back(13);

	mIndices.emplace_back(12);
	mIndices.emplace_back(15);
	mIndices.emplace_back(14);

	//Up
	mIndices.emplace_back(16);
	mIndices.emplace_back(17);
	mIndices.emplace_back(18);

	mIndices.emplace_back(16);
	mIndices.emplace_back(18);
	mIndices.emplace_back(19);

	//Down
	mIndices.emplace_back(20);
	mIndices.emplace_back(22);
	mIndices.emplace_back(21);

	mIndices.emplace_back(20);
	mIndices.emplace_back(23);
	mIndices.emplace_back(22);

	createNormal();

	mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
		mIndices.data(), (UINT)mIndices.size());
}

void Cube::createNormal()
{
	for (UINT i = 0; i < mIndices.size() / 3; i++)
	{
		UINT index0 = mIndices[i * 3 + 0];
		UINT index1 = mIndices[i * 3 + 1];
		UINT index2 = mIndices[i * 3 + 2];

		Vector3 v0 = mVertices[index0].position;
		Vector3 v1 = mVertices[index1].position;
		Vector3 v2 = mVertices[index2].position;

		Vector3 A = v1 - v0;
		Vector3 B = v2 - v0;

		Vector3 normal = Vector3::Cross(A, B).Normal();

		mVertices[index0].normal = normal;
		mVertices[index1].normal = normal;
		mVertices[index2].normal = normal;
	}
}
