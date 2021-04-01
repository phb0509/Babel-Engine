#pragma once

class SpriteEffect : public Particle
{
private:
	class SpriteBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float2 maxFrame = Float2(0, 0);
			Float2 curFrame = Float2(0, 0);
		}data;

		SpriteBuffer() : ConstBuffer(&data, sizeof(Data))
		{
		}
	};

	SpriteBuffer* spriteBuffer;
	VertexSize* vertices;

	float time;
	float speed;
	UINT curFrameCount;
	UINT maxFrameCount;

	Float2 size;

public:
	SpriteEffect(wstring diffuseFile, Float2 maxFrame);
	~SpriteEffect();

	// Particle을(를) 통해 상속됨
	virtual void Create() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Play(Vector3 position) override;
};