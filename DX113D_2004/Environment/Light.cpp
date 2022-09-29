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
	mLightBuffer(nullptr),
	mIndex(0)
{
	mSphere = new Sphere(L"Lighting");
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
	mLightBuffer(nullptr),
	mIndex(0)
{
	mSphere = new Sphere(L"Lighting");
}

Light::~Light()
{
	GM->SafeDelete(mSphere);
}

void Light::Update()
{
	UpdateWorld();
	updateLightInfo();

	mDirection = this->GetForwardVector();
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
		ImGui::SliderInt("Type", (int*)&mType, 0, 3); 
		ImGui::SliderFloat3("Rotation", (float*)&this->mRotation, -XM_PI, XM_PI);
		//ImGui::SliderFloat3("Position", (float*)&this->mPosition, -100, 100);
		//ImGui::InputFloat3("Rotation", (float*)&this->mRotation);
		ImGui::InputFloat3("Position", (float*)&this->mPosition);
		ImGui::SliderFloat3("Direction", (float*)&this->mDirection, -1, 1);
		ImGui::SliderFloat("Range", (float*)&mRange, 0, 100); // 라이트 크기?
		ImGui::ColorEdit4("Color", (float*)&mColor);	
		ImGui::SliderFloat("Inner", (float*)&mInner, 0, 90);
		ImGui::SliderFloat("Outer", (float*)&mOuter, 0, 180);
		ImGui::SliderFloat("Length", (float*)&mLength, 0, 180);

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
	mLightInfo.direction = this->GetForwardVector();
	mLightInfo.type = this->mType;
	mLightInfo.range = this->mRange;
	mLightInfo.inner = this->mInner;
	mLightInfo.outer = this->mOuter;
	mLightInfo.length = this->mLength;
	mLightInfo.active = this->mActive;
}
