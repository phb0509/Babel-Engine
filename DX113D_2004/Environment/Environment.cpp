#include "Framework.h"

Environment::Environment() 
{
	mSamplerState = new SamplerState();
	mSamplerState->SetState();

	mLightBuffer = new LightBuffer();
	mLightBuffer->Add();
}

Environment::~Environment()
{
	delete mLightBuffer;
	delete mSamplerState;
}

void Environment::PostRender()
{
	/*if (lightBuffer->data.lightCount >= 1)
	{
		mSun->mPosition = lightBuffer->data.lights[0].position;
	}*/
	
	//showLightInformation();
}

void Environment::showLightInformation()
{
	ImGui::Begin("LightInformation");

	ImGui::Text("LightInfo");
	ImGui::ColorEdit4("AmbientColor", (float*)&mLightBuffer->data.ambient);
	ImGui::ColorEdit4("AmbientCeilColor", (float*)&mLightBuffer->data.ambientCeil);

	if (ImGui::Button("AddLight"))
	{
		mLightBuffer->Add();
	}

	for (UINT i = 0; i < mLightBuffer->data.lightCount; i++)
	{
		string name = "Light " + to_string(i);
		if (ImGui::BeginMenu(name.c_str()))
		{
			ImGui::Checkbox("Active", (bool*)&mLightBuffer->data.lights[i].active);
			ImGui::SliderInt("Type", (int*)&mLightBuffer->data.lights[i].type, 0, 3); // 라이트 타입
			ImGui::SliderFloat3("Direction", (float*)&mLightBuffer->data.lights[i].direction, -1, 1); // 라이트 방향
			ImGui::SliderFloat3("LightPosition", (float*)&mLightBuffer->data.lights[i].position, -100, 100); // 라이트 위치
			ImGui::SliderFloat("LightRange", (float*)&mLightBuffer->data.lights[i].range, 0, 100); // 라이트 크기?
			ImGui::ColorEdit4("LightColor", (float*)&mLightBuffer->data.lights[i].color);			// 라이트색깔
			ImGui::SliderFloat("LightInner", (float*)&mLightBuffer->data.lights[i].inner, 0, 90);
			ImGui::SliderFloat("LightOuter", (float*)&mLightBuffer->data.lights[i].outer, 0, 180);
			ImGui::SliderFloat("LightLength", (float*)&mLightBuffer->data.lights[i].length, 0, 180);

			ImGui::EndMenu();
		}
	}

	ImGui::End();
}

void Environment::Set()
{
	SetViewport();

	mLightBuffer->SetPSBuffer(0);
}

void Environment::SetViewport(UINT width, UINT height)
{
	mViewport.Width = (float)width;
	mViewport.Height = (float)height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	DEVICECONTEXT->RSSetViewports(1, &mViewport);
}

Vector3 Environment::GetLightPosition()
{
	Vector3 t;
	t.x = mLightBuffer->data.lights[0].position.x;
	t.y = mLightBuffer->data.lights[0].position.y;
	t.z = mLightBuffer->data.lights[0].position.z;

	return t;
}