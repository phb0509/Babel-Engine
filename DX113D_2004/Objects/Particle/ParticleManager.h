#pragma once

class ParticleManager : public Singleton<ParticleManager>
{
private:
	friend class Singleton;

	map<string, vector<Particle*>> totalParticle;

	ParticleManager();
	~ParticleManager();

public:
	void Update();
	void Render();

	void Play(string key, Vector3 position);

	void CreateSpark();
	void CreateSpriteEffect(string key, wstring diffuseFile, Float2 maxFrame, UINT poolCount);
};