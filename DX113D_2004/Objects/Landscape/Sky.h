#pragma once

class Sky
{
private:
	class SkyBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float4 center;
			Float4 apex;

			float height;
			float padding[3];
		}data;

		SkyBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.center = { 1.0f, 0.6f, 0.2f, 1.0f };
			data.apex = { 0.3f, 0.5f, 1.0f, 1.0f };

			data.height = 5.0f;
		}
	};

	Sphere* sphere;	

	SkyBuffer* buffer;

	RasterizerState* rasterizerState[2];
	DepthStencilState* depthState[2];
public:
	Sky();
	~Sky();

	void Render();

	void PostRender();
};