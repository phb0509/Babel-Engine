#include "Framework.h"

Snow::Snow() : Particle(L"Textures/Effect/Snow.png")
{
	material->SetShader(L"ParticleSnow");
	geometryShader = Shader::AddGS(L"ParticleSnow");

	buffer = new SnowBuffer();

	Create();

	blendState[1]->Alpha(true);
}

Snow::~Snow()
{
	delete buffer;
}

void Snow::Create()
{
	vertices = new VertexSnow[particleCount];

	Float3 scale = buffer->data.size;

	for (UINT i = 0; i < particleCount; i++)
	{
		float size;
		size = Random(0.5f, 2.0f);		

		Float3 position;
		position.x = Random(-scale.x, scale.x);
		position.y = Random(-scale.y, scale.y);
		position.z = Random(-scale.z, scale.z);

		Float2 random;
		random.x = Random(0.0f, 1.0f);
		random.y = Random(0.0f, 1.0f);

		vertices[i].position = position;
		vertices[i].scale = size;
		vertices[i].random = random;
	}

	vertexBuffer = new VertexBuffer(vertices, sizeof(VertexSnow), particleCount);
}

void Snow::Update()
{
	buffer->data.time += DELTA;
}

void Snow::Render()
{
	buffer->SetVSBuffer(10);
	Particle::Render();
}

void Snow::PostRender()
{
	ImGui::SliderFloat3("Velocity", (float*)&buffer->data.velocity, -50, 50);
	ImGui::ColorEdit4("Color", (float*)&buffer->data.color);
	ImGui::SliderFloat("Distance", (float*)&buffer->data.distance, 0, 2000);
	ImGui::SliderFloat3("Origin", (float*)&buffer->data.origin, -50, 50);
	ImGui::SliderFloat("Turbulence", (float*)&buffer->data.turbulence, 0, 20);
}
