#include "Framework.h"

Light::Light() :
	mColor(1.0f, 1.0f, 1.0f, 1.0f),
	mDirection(0.0f, -1.0f, 1.0f),
	mType(LightType::DIRECTIONAL),
	mRange(80.0f),
	mInner(55.0f),
	mOuter(65.0f),
	mLength(50.0f),
	mActive(true),
	mSphere(nullptr),
	mLightBuffer(nullptr),
	mIndex(0)
{
	mSphere = new Sphere(L"Lighting");
	//mSphere->mScale = { 10.0f,10.0f,10.0f };
}

Light::Light(LightType lightType):
	mColor(1.0f, 1.0f, 1.0f, 1.0f),
	mDirection(0.0f, -1.0f, 1.0f),
	mType(lightType),
	mRange(80.0f),
	mInner(55.0f),
	mOuter(65.0f),
	mLength(50.0f),
	mActive(true),
	mSphere(nullptr),
	mLightBuffer(nullptr),
	mIndex(0)
{
	mSphere = new Sphere(L"Lighting");
	//mSphere->mScale = { 10.0f,10.0f,10.0f };
}

Light::~Light()
{
	delete mSphere;
}

void Light::Update()
{
	UpdateWorld();
	updateLightInfo();

	mSphere->mPosition = this->mPosition;
	mSphere->Update();
}

void Light::Render()
{
	mSphere->Render();
}

void Light::PostRender()
{
	ImGui::Begin("LightInformation");
	string name = mTag;

	if (ImGui::BeginMenu(name.c_str()))
	{
		ImGui::Checkbox("Active", (bool*)&mActive);
		ImGui::SliderInt("Type", (int*)&mType, 0, 3); // 라이트 타입
		ImGui::SliderFloat3("Direction", (float*)&mDirection, -1, 1); // 라이트 방향
		ImGui::SliderFloat3("LightPosition", (float*)&this->mPosition, -100, 100); // 라이트 위치
		ImGui::SliderFloat("LightRange", (float*)&mRange, 0, 100); // 라이트 크기?
		ImGui::ColorEdit4("LightColor", (float*)&mColor);	// 라이트색깔
		ImGui::SliderFloat("LightInner", (float*)&mInner, 0, 90);
		ImGui::SliderFloat("LightOuter", (float*)&mOuter, 0, 180);
		ImGui::SliderFloat("LightLength", (float*)&mLength, 0, 180);

		ImGui::EndMenu();
	}

	ImGui::End();
}

void Light::Delete()
{
	mLightBuffer->DeleteLight(mIndex);
	delete this;
}

void Light::updateLightInfo()
{
	mLightInfo.position = this->mPosition;
	mLightInfo.color = this->mColor;
	mLightInfo.direction = this->mDirection;
	mLightInfo.type = this->mType;
	mLightInfo.range = this->mRange;
	mLightInfo.inner = this->mInner;
	mLightInfo.outer = this->mOuter;
	mLightInfo.length = this->mLength;
	mLightInfo.active = this->mActive;
}
