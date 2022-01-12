#pragma once

class TestScene : public Scene
{
private:

public:
	TestScene();
	~TestScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	ID3D11Texture2D* Texture = nullptr;
	ID3D11Texture2D* Staging = nullptr;
	
	float* Read;
	float* Data;

	int const Width = 100;
	int const Height = 100;
};