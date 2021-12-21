#include "Framework.h"

SpriteEffect::SpriteEffect(wstring diffuseFile, Float2 maxFrame)
	: Particle(diffuseFile), time(0), speed(1), curFrameCount(0),
	size(10, 10)
{
	particleCount = 1;

	material->SetShader(L"ParticleSprite");
	geometryShader = Shader::AddGS(L"ParticleSprite");

	spriteBuffer = new SpriteBuffer();
	spriteBuffer->data.maxFrame = maxFrame;
	maxFrameCount = (UINT)(maxFrame.x * maxFrame.y);

	Create();
}

SpriteEffect::~SpriteEffect()
{
	delete spriteBuffer;
	delete vertices;
}

void SpriteEffect::Create()
{
	vertices = new VertexSize();

	vertices->size = size;

	vertexBuffer = new VertexBuffer(vertices, sizeof(VertexSize), particleCount);
}

void SpriteEffect::Update()
{
	if (!mbIsActive)
		return;

	time += speed * DELTA;

	if (time > 0.1f)
	{
		curFrameCount++;
		spriteBuffer->data.curFrame.x = curFrameCount % (UINT)spriteBuffer->data.maxFrame.x;
		spriteBuffer->data.curFrame.y = curFrameCount / (UINT)spriteBuffer->data.maxFrame.x;
		time = 0.0f;
	}

	if (curFrameCount >= maxFrameCount)
		Stop();

	UpdateWorld();
}

void SpriteEffect::Render()
{
	spriteBuffer->SetPSBuffer(10);

	Particle::Render();
}

void SpriteEffect::PostRender()
{
	ImGui::SliderFloat("Speed", &speed, 0.1f, 10.0f);
	ImGui::SliderFloat2("Size", (float*)&size, 1.0f, 20.0f);
}

void SpriteEffect::Play(Vector3 position)
{
	mbIsActive = true;

	time = 0.0f;
	curFrameCount = 0;

	vertices->position = position;
	vertices->size = size;

	vertexBuffer->Update(vertices, particleCount);
}
