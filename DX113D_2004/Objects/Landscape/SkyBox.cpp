#include "Framework.h"

SkyBox::SkyBox()
{
	sphere = new Sphere(L"SkyBox");

	cubeMap = Texture::Add(L"Textures/Landscape/NightSkyCube.dds"); // view dimension 타입 안맞는다고 에러뜨는듯?(추측임)
	//cubeMap = Texture::Add(L"Textures/Landscape/Snow_ENV.dds");
	
	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FrontCounterClockwise(true);	

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthEnable(false);
}

SkyBox::~SkyBox()
{
	delete sphere;

	delete rasterizerState[0];
	delete rasterizerState[1];

	delete depthState[0];
	delete depthState[1];
}

void SkyBox::Render()
{
	cubeMap->PSSet(10);

	rasterizerState[1]->SetState();
	depthState[1]->SetState();

	sphere->Render();

	rasterizerState[0]->SetState();
	depthState[0]->SetState();
}
