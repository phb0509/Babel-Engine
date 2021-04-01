#include "Framework.h"

ParticleManager::ParticleManager()
{
	CreateSpark();
	CreateSpriteEffect("exp", L"Textures/Effect/explosion.png", Float2(5, 3), 30);
}

ParticleManager::~ParticleManager()
{
	for (auto particles : totalParticle)
	{
		for (Particle* particle : particles.second)
		{
			delete particle;
		}
	}
}

void ParticleManager::Update()
{
	for (auto particles : totalParticle)
	{
		for (Particle* particle : particles.second)
		{
			particle->Update();
		}
	}
}

void ParticleManager::Render()
{
	for (auto particles : totalParticle)
	{
		for (Particle* particle : particles.second)
		{
			particle->Render();
		}
	}
}

void ParticleManager::Play(string key, Vector3 position)
{
	if (totalParticle.count(key) == 0)
		return;

	for (Particle* particle : totalParticle[key])
	{
		if (!particle->isActive)
		{
			particle->Play(position);
			return;
		}
	}
}

void ParticleManager::CreateSpark()
{
	UINT poolCount = 50;

	vector<Particle*> particles;

	for (UINT i = 0 ; i < poolCount; i++)
	{
		particles.emplace_back(new Spark());
	}

	totalParticle["spark"] = particles;
}

void ParticleManager::CreateSpriteEffect(string key, wstring diffuseFile, Float2 maxFrame, UINT poolCount)
{	
	vector<Particle*> particles;

	for (UINT i = 0; i < poolCount; i++)
	{
		particles.emplace_back(new SpriteEffect(diffuseFile, maxFrame));
	}

	totalParticle[key] = particles;
}
