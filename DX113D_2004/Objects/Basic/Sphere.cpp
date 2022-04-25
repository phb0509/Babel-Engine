#include "Framework.h"

Sphere::Sphere(wstring shaderFile, float radius, UINT sliceCount, UINT stackCount)
	: radius(radius), sliceCount(sliceCount), stackCount(stackCount)
{
	material = new Material(shaderFile);
	typeBuffer = new TypeBuffer();
	CreateMesh();

	material->GetBuffer()->data.specular.w = 100.0f;
}

Sphere::~Sphere()
{
	delete mesh;
	delete material;
	delete typeBuffer;
}

void Sphere::Update()
{
	UpdateWorld();
}

void Sphere::Render()
{
	mesh->IASet();
	SetWorldBuffer();
	material->Set();

	typeBuffer->SetVSBuffer(5);

	DEVICECONTEXT->DrawIndexed(indices.size(), 0, 0);
}

void Sphere::PostRender()
{
	ImGui::Begin("Sphere");
	ImGui::InputFloat3("Position", (float*)&this->mPosition);
	ImGui::InputFloat3("Rotation", (float*)&this->mRotation);
	ImGui::InputFloat3("Scale", (float*)&this->mScale);
	ImGui::End();
}

void Sphere::CreateMesh()
{
	float phiStep = XM_PI / stackCount;
	float thetaStep = XM_2PI / sliceCount;

	for (UINT i = 0; i <= stackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= sliceCount; j++)
		{
			float theta = j * thetaStep;

			VertexType vertex;

			vertex.normal.x = sin(phi) * cos(theta);
			vertex.normal.y = cos(phi);
			vertex.normal.z = sin(phi) * sin(theta);

			vertex.position = Vector3(vertex.normal) * radius;

			vertex.uv.x = (float)j / sliceCount;
			vertex.uv.y = (float)i / stackCount;

			vertices.emplace_back(vertex);
		}
	}

	for (UINT i = 0; i < stackCount; i++)
	{
		for(UINT j = 0; j < sliceCount; j++)
		{
			indices.emplace_back((sliceCount + 1) * i + j);
			indices.emplace_back((sliceCount + 1) * i + j + 1);
			indices.emplace_back((sliceCount + 1) * (i + 1) + j);

			indices.emplace_back((sliceCount + 1) * (i + 1) + j);
			indices.emplace_back((sliceCount + 1) * i + j + 1);
			indices.emplace_back((sliceCount + 1) * (i + 1) + j + 1);
		}
	}

	CreateTangent();

	mesh = new Mesh(vertices.data(), sizeof(VertexType), vertices.size(),
		indices.data(), indices.size());
}

void Sphere::CreateTangent()
{
	for (UINT i = 0; i < indices.size() / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexType vertex0 = vertices[index0];
		VertexType vertex1 = vertices[index1];
		VertexType vertex2 = vertices[index2];

		Vector3 p0 = vertex0.position;
		Vector3 p1 = vertex1.position;
		Vector3 p2 = vertex2.position;

		Float2 uv0 = vertex0.uv;
		Float2 uv1 = vertex1.uv;
		Float2 uv2 = vertex2.uv;

		Vector3 e0 = p1 - p0; // 0 -> 1 บคลอ
		Vector3 e1 = p2 - p0; // 0 -> 2 บคลอ

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;

		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		//float d = 1.0f / (u0 * v1 - v0 * u1);
		//Vector3 tangent = d * (v1 * e0 - v0 * e1);
		Vector3 tangent = (v1 * e0 - v0 * e1);

		vertices[index0].tangent = tangent + vertices[index0].tangent;
		vertices[index1].tangent = tangent + vertices[index1].tangent;
		vertices[index2].tangent = tangent + vertices[index2].tangent;
	}

	for (VertexType& vertex : vertices)
	{
		Vector3 t = vertex.tangent;
		Vector3 n = vertex.normal;

		Vector3 temp = (t - n * Vector3::Dot(n, t)).Normal();

		vertex.tangent = temp;
	}
}
