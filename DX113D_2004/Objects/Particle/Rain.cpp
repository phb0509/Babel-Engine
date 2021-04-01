#include "Framework.h"

Rain::Rain()
	: Particle(L"Textures/Effect/Rain.png")
{
	material->SetShader(L"ParticleRain");
	geometryShader = Shader::AddGS(L"ParticleRain");

	buffer = new RainBuffer;
	Create();

	blendState[1]->Alpha(true);	
}

Rain::~Rain()
{
	delete buffer;
}

void Rain::Create()
{
	vertices = new VertexSize[particleCount];

	Float3 scale = buffer->data.size;

	for (UINT i = 0; i < particleCount; i++)
	{
		Float2 size;
		size.x = Random(0.1f, 0.4f);
		size.y = Random(4.0f, 8.0f);

		Float3 position;
		position.x = Random(-scale.x, scale.x);
		position.y = Random(-scale.y, scale.y);
		position.z = Random(-scale.z, scale.z);

		vertices[i].position = position;
		vertices[i].size = size;
	}

	vertexBuffer = new VertexBuffer(vertices, sizeof(VertexSize), particleCount);
}

void Rain::Update()
{
	buffer->data.time += DELTA;
}

void Rain::Render()
{
	buffer->SetVSBuffer(10);
	Particle::Render();
}

void Rain::PostRender()
{
	ImGui::SliderFloat3("Velocity", (float*)&buffer->data.velocity, -50, 50);
	ImGui::ColorEdit4("Color", (float*)&buffer->data.color);
	ImGui::SliderFloat("Distance", (float*)&buffer->data.distance, 0, 2000);
	ImGui::SliderFloat3("Origin", (float*)&buffer->data.origin, -50, 50);
}
