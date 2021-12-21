#include "Framework.h"

Spark::Spark()
	: Particle(L"Textures/Landscape/star.png"),
	minRadius(5.0f), maxRadius(10.0f), minSize(0.1f), maxSize(1.0f)
{
	material->SetShader(L"ParticleSpark");
	geometryShader = Shader::AddGS(L"ParticleSpark");

	sparkBuffer = new SparkBuffer();
	startColor = new ColorBuffer();
	endColor = new ColorBuffer();

	Create();
}

Spark::~Spark()
{
	delete sparkBuffer;
	delete[] vertices;

	delete startColor;
	delete endColor;
}

void Spark::Create()
{
	vertices = new VertexParticle[MAX_COUNT];

	UpdateParticle();

	vertexBuffer = new VertexBuffer(vertices, sizeof(VertexParticle), MAX_COUNT);

	sparkBuffer->data.duration = 2.0f;
}

void Spark::Update()
{
	if (!mbIsActive)
		return;

	sparkBuffer->data.time += DELTA;

	if (sparkBuffer->data.time > sparkBuffer->data.duration)
		Stop();
}

void Spark::Render()
{
	sparkBuffer->SetVSBuffer(10);
	startColor->SetPSBuffer(10);
	endColor->SetPSBuffer(11);

	Particle::Render();
}

void Spark::PostRender()
{
	ImGui::SliderFloat3("Direction", (float*)&sparkBuffer->data.direction, -5, 5);
	ImGui::SliderFloat("Duration", &sparkBuffer->data.duration, 0.0f, 10.0f);
	ImGui::SliderInt("Count", (int*)&particleCount, 1, 1000);
	ImGui::SliderFloat("MinRadius", &minRadius, 1.0f, 30.0f);
	ImGui::SliderFloat("MaxRadius", &maxRadius, 1.0f, 30.0f);
	ImGui::SliderFloat("MinSize", &minSize, 0.1f, 1.0f);
	ImGui::SliderFloat("MaxSize", &maxSize, 0.1f, 1.0f);
	ImGui::ColorEdit4("StartColor", (float*)&startColor->data.color);
	ImGui::ColorEdit4("EndColor", (float*)&endColor->data.color);
}

void Spark::Play(Vector3 position)
{
	sparkBuffer->data.time = 0.0f;
	Particle::Play(position);

	UpdateParticle();
	vertexBuffer->Update(vertices, particleCount);
}

void Spark::UpdateParticle()
{
	for (UINT i = 0; i < particleCount; i++)
	{
		float size = Random(minSize, maxSize);
		vertices[i].size = Float2(size, size);

		float radius = Random(minRadius, maxRadius);

		Vector3 velocity = Vector3(0, 0, radius);

		Float3 rot;
		rot.x = Random(0.0f, XM_2PI);
		rot.y = Random(0.0f, XM_2PI);
		rot.z = Random(0.0f, XM_2PI);

		Matrix matRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);

		XMStoreFloat3(&vertices[i].velocity, XMVector3TransformCoord(velocity.data, matRot));
	}
}
