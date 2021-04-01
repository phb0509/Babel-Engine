#pragma once

class SkyBox
{
private:
	Sphere* sphere;
	Texture* cubeMap;
	
	RasterizerState* rasterizerState[2];
	DepthStencilState* depthState[2];
public:
	SkyBox();
	~SkyBox();

	void Render();
};