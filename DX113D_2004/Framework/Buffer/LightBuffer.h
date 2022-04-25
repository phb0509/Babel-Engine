#pragma once

#define MAX_LIGHT 10

class Light;

class LightBuffer : public ConstBuffer
{
public:
	struct Data
	{
		LightInfo lights[MAX_LIGHT];
		UINT lightCount;
		float padding[3];

		Float4 ambient;
		Float4 ambientCeil;
	};

	LightBuffer();
	~LightBuffer();

	void Update();
	void PostRender();
	void Add(Light* light);
	void DeleteLight(int index);

private:
	void UpdateBufferData();

private:
	Data mData;
	vector<Light*> mLights;

};