#include "Framework.h"

LightBuffer::LightBuffer():
	ConstBuffer(&mData, sizeof(Data))
{
	mData.lightCount = 0;
	mData.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	mData.ambientCeil = { 0.1f, 0.1f, 0.1f, 1.0f };
}

LightBuffer::~LightBuffer()
{
}

void LightBuffer::Update()
{
	UpdateBufferData();
}

void LightBuffer::PostRender()
{
	ImGui::Begin("LightBuffer");
	ImGui::ColorEdit4("AmbientColor", (float*)&mData.ambient);
	ImGui::ColorEdit4("AmbientCeilColor", (float*)&mData.ambientCeil);
	ImGui::SliderFloat4("Ambient", (float*)&mData.ambient, 0.0f, 1.0f);
	ImGui::SliderFloat4("AmbientCeil", (float*)&mData.ambientCeil, 0.0f, 1.0f);
	ImGui::End();
}

void LightBuffer::UpdateBufferData()
{
	mData.lightCount = mLights.size();

	for (int i = 0; i < mLights.size(); i++)
	{
		mLights[i]->SetIndex(i);
		mData.lights[i] = mLights[i]->GetLightInfo();
	}
}

void LightBuffer::Add(Light* light)
{
	light->SetLightBuffer(this);
	mLights.push_back(light);
}

void LightBuffer::DeleteLight(int index)
{
	mLights.erase(mLights.begin() + index);
}