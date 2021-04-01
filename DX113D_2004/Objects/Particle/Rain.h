#pragma once

class Rain : public Particle
{
private:
	class RainBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float3 velocity = Float3(0, -1, 0);
			float distance = 1000.0f;

			Float4 color = Float4(1, 1, 1, 1);

			Float3 origin = Float3(0, 0, 0);
			float time = 0.0f;

			Float3 size = Float3(50, 50, 50);
			float padding;
		}data;

		RainBuffer() : ConstBuffer(&data, sizeof(Data))
		{
		}
	};

	RainBuffer* buffer;
	VertexSize* vertices;

public:
	Rain();
	~Rain();
		
	virtual void Create() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostRender() override;
};