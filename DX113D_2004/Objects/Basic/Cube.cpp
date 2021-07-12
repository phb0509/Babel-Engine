#include "Framework.h"

Cube::Cube()
{
	material = new Material(L"Diffuse");
	material->SetDiffuseMap(L"Textures/test.jpg");

	Create();
}

Cube::~Cube()
{
	delete material;
	delete mesh;

}

void Cube::Update()
{	
	UpdateWorld();
}

void Cube::Render()
{
	mesh->IASet();

	SetWorldBuffer();

	material->Set();

	DEVICECONTEXT->DrawIndexed((UINT)indices.size(), 0, 0);
}

void Cube::Create()
{
	vertices.resize(24);
	//Front
	vertices[0].position = { -1, -1, -1 };
	vertices[1].position = { -1, +1, -1 };
	vertices[2].position = { +1, +1, -1 };
	vertices[3].position = { +1, -1, -1 };

	//Back
	vertices[4].position = { -1, -1, +1 };
	vertices[5].position = { -1, +1, +1 };
	vertices[6].position = { +1, +1, +1 };
	vertices[7].position = { +1, -1, +1 };

	//Right
	vertices[8].position = { +1, -1, -1 };
	vertices[9].position = { +1, +1, -1 };
	vertices[10].position = { +1, +1, +1 };
	vertices[11].position = { +1, -1, +1 };

	//Left
	vertices[12].position = { -1, -1, -1 };
	vertices[13].position = { -1, +1, -1 };
	vertices[14].position = { -1, +1, +1 };
	vertices[15].position = { -1, -1, +1 };

	//Up
	vertices[16].position = { -1, +1, -1 };
	vertices[17].position = { -1, +1, +1 };
	vertices[18].position = { +1, +1, +1 };
	vertices[19].position = { +1, +1, -1 };

	//Down
	vertices[20].position = { -1, -1, -1 };
	vertices[21].position = { -1, -1, +1 };
	vertices[22].position = { +1, -1, +1 };
	vertices[23].position = { +1, -1, -1 };

	for (UINT i = 0; i < 6; i++)
	{
		vertices[i * 4 + 0].uv = { 0, 1 };
		vertices[i * 4 + 1].uv = { 0, 0 };
		vertices[i * 4 + 2].uv = { 1, 0 };
		vertices[i * 4 + 3].uv = { 1, 1 };
	}	

	//Front
	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);

	indices.emplace_back(0);
	indices.emplace_back(2);
	indices.emplace_back(3);

	//Back
	indices.emplace_back(4);
	indices.emplace_back(6);
	indices.emplace_back(5);

	indices.emplace_back(4);
	indices.emplace_back(7);
	indices.emplace_back(6);

	//Right
	indices.emplace_back(8);
	indices.emplace_back(9);
	indices.emplace_back(10);

	indices.emplace_back(8);
	indices.emplace_back(10);
	indices.emplace_back(11);

	//Left
	indices.emplace_back(12);
	indices.emplace_back(14);
	indices.emplace_back(13);

	indices.emplace_back(12);
	indices.emplace_back(15);
	indices.emplace_back(14);

	//Up
	indices.emplace_back(16);
	indices.emplace_back(17);
	indices.emplace_back(18);

	indices.emplace_back(16);
	indices.emplace_back(18);
	indices.emplace_back(19);

	//Down
	indices.emplace_back(20);
	indices.emplace_back(22);
	indices.emplace_back(21);

	indices.emplace_back(20);
	indices.emplace_back(23);
	indices.emplace_back(22);

	CreateNormal();

	mesh = new Mesh(vertices.data(), sizeof(VertexType), (UINT)vertices.size(),
		indices.data(), (UINT)indices.size());
}

void Cube::CreateNormal()
{
	for (UINT i = 0; i < indices.size() / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		Vector3 v0 = vertices[index0].position;
		Vector3 v1 = vertices[index1].position;
		Vector3 v2 = vertices[index2].position;

		Vector3 A = v1 - v0;
		Vector3 B = v2 - v0;

		Vector3 normal = Vector3::Cross(A, B).Normal();

		vertices[index0].normal = normal;
		vertices[index1].normal = normal;
		vertices[index2].normal = normal;
	}
}
