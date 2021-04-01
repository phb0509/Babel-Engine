#include "Framework.h"

Sky::Sky()
{
	sphere = new Sphere(L"Sky");	
	buffer = new SkyBuffer();

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FrontCounterClockwise(true);

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthEnable(false);
}

Sky::~Sky()
{
	delete sphere;
	delete buffer;

	delete rasterizerState[0];
	delete rasterizerState[1];

	delete depthState[0];
	delete depthState[1];
}

void Sky::Render()
{
	buffer->SetPSBuffer(10);

	rasterizerState[1]->SetState();
	depthState[1]->SetState();

	sphere->Render();

	rasterizerState[0]->SetState();
	depthState[0]->SetState();	
}

void Sky::PostRender()
{
	ImGui::Text("SkyOption");
	ImGui::ColorEdit4("Center", (float*)&buffer->data.center);
	ImGui::ColorEdit4("Apex", (float*)&buffer->data.apex);
	ImGui::SliderFloat("Height", &buffer->data.height, 1.0f, 10.0f);
}
