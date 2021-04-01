#pragma once

class Spark : public Particle
{
private:
	struct VertexParticle
	{
		Float3 position;
		Float2 size;
		Float3 velocity;

		VertexParticle()
			: position(0, 0, 0), size(0, 0), velocity(0, 0, 0)
		{
		}
	};

	class SparkBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float3 direction = Float3(0, 0, 0);
			float duration = 0.0f;

			float time = 0.0f;
			float padding[3];
		}data;

		SparkBuffer() : ConstBuffer(&data, sizeof(Data))
		{			
		}
	};

	SparkBuffer* sparkBuffer;
	VertexParticle* vertices;

	float minRadius;
	float maxRadius;

	float minSize;
	float maxSize;

	ColorBuffer* startColor;
	ColorBuffer* endColor;
public:
	Spark();
	~Spark();

	virtual void Create() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;

	virtual void Play(Vector3 position) override;

	void UpdateParticle();
};