#pragma once

class Snow : public Particle
{
private:
	class SnowBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float3 velocity = Float3(0, -5, 0);
			float distance = 50;

			Float4 color = Float4(1, 1, 1, 1);

			Float3 origin = Float3(0, 0, 0);
			float time = 0.0f;

			Float3 size = Float3(50, 50, 50);
			float turbulence = 5.0f;
		}data;

		SnowBuffer() : ConstBuffer(&data, sizeof(Data))
		{
		}
	};

	struct VertexSnow
	{
		Float3 position;
		float scale;
		Float2 random;
	};

	SnowBuffer* buffer;
	VertexSnow* vertices;

public:
	Snow();
	~Snow();

	virtual void Create() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;
};