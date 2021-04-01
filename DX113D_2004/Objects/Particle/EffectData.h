#pragma once

class EffectData
{
public:
	enum class BlendType
	{		
		ADDITIVE,
		ALPHABLEND
	}type = BlendType::ADDITIVE;

	bool isLoop = false;

	wstring textureFile = L"Textures/Effect/Snow.png";

	UINT maxParticles = 100;

	float readyTime = 1.0f;
	float readyRandomTime = 0;

	float startVelocity = 1;
	float endVelocity = 1;

	float minHorizontalVelocity = 0;
	float maxHorizontalVelocity = 0;

	float minVerticalVelocity = 0;
	float maxVerticalVelocity = 0;

	Vector3 gravity = Vector3(0, 0, 0);

	Float4 minColor = Float4(1, 1, 1, 1);
	Float4 maxColor = Float4(1, 1, 1, 1);

	float minRotateSpeed = 0;
	float maxRotateSpeed = 0;

	float minStartSize = 100;
	float maxStartSize = 100;

	float minEndSize = 100;
	float maxEndSize = 100;
};