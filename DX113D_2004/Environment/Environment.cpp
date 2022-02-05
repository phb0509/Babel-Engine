#include "Framework.h"

Environment::Environment() :
	mbIsTargetCamera(false),
	mTarget(nullptr),
	mbIsEnabledTargetCamera(true),
	mbIsEnabledWorldCamera(true)
{
	createPerspectiveBuffer();
	createOrthographicBuffer();

	mSamplerState = new SamplerState();
	mSamplerState->SetState();

	mTargetCamera = new Camera();
	mTargetCamera->CreateFrustum();
	mWorldCamera = new Camera();

	mLightBuffer = new LightBuffer();
	mLightBuffer->Add();
}

Environment::~Environment()
{
	delete mPerspectiveProjectionBuffer;
	delete mOrthographicProjectionBuffer;
	delete mLightBuffer;
	delete mSamplerState;
	//delete mSun;
}

void Environment::PostRender()
{
	//mSun->Update();
	//mSun->Render();

	/*if (lightBuffer->data.lightCount >= 1)
	{
		mSun->mPosition = lightBuffer->data.lights[0].position;
	}*/
	
	ImGui::Begin("Camera Info");
	ImGui::Text("SelectCamera");

	if (ImGui::Button("TargetCamera"))
	{
		mbIsTargetCamera = true;
		mTargetCamera->SetIsMouseInputing(true); 
	}

	if (ImGui::Button("WorldCamera"))
	{
		mbIsTargetCamera = false;
		mWorldCamera->mPosition = Vector3(110.0f, 175.0f, -40.0f);
		mWorldCamera->mRotation = Vector3(1.0f, 0.0f, 0.0f);
		mTargetCamera->SetIsMouseInputing(false); //// ����ī�޶������ �� Ÿ��ī�޶��� ���콺�̵����� ���� ȸ���� ��������.
	}

	SpacingRepeatedly(2);

	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mWorldCamera->mPosition.x, mWorldCamera->mPosition.y, mWorldCamera->mPosition.z);
	SpacingRepeatedly(1);
	ImGui::Text("WorldCameraRotation : %.1f,  %.1f,  %.1f", mWorldCamera->mRotation.x, mWorldCamera->mRotation.y, mWorldCamera->mRotation.z);
	SpacingRepeatedly(1);
	ImGui::Text("MousePosition : %d, %d", (int)MOUSEPOS.x, (int)MOUSEPOS.y);
	SpacingRepeatedly(2);

	/*ImGui::Text("PlayerPosition : %.1f, %.1f, %.1f", GM->GetPlayer()->mPosition.x, GM->GetPlayer()->mPosition.y, GM->GetPlayer()->mPosition.z);
	SpacingRepeatedly();
	ImGui::Text("TargetCameraPosition : %.1f,  %.1f,  %.1f", mTargetCamera->mPosition.x, mTargetCamera->mPosition.y, mTargetCamera->mPosition.z);
	SpacingRepeatedly();
	ImGui::Text("WorldCameraPosition : %.1f,  %.1f,  %.1f", mWorldCamera->mPosition.x, mWorldCamera->mPosition.y, mWorldCamera->mPosition.z);
	SpacingRepeatedly();
	ImGui::Text("WorldCameraForward : %.1f,  %.1f,  %.1f", mWorldCamera->Forward().x, mWorldCamera->Forward().y, mWorldCamera->Forward().z);*/

	ImGui::End();
	//showLightInformation();

	mTargetCamera->PostRender();
	mWorldCamera->PostRender();
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
			ImGui::SliderInt("Type", (int*)&mLightBuffer->data.lights[i].type, 0, 3); // ����Ʈ Ÿ��
			ImGui::SliderFloat3("Direction", (float*)&mLightBuffer->data.lights[i].direction, -1, 1); // ����Ʈ ����
			ImGui::SliderFloat3("LightPosition", (float*)&mLightBuffer->data.lights[i].position, -100, 100); // ����Ʈ ��ġ
			ImGui::SliderFloat("LightRange", (float*)&mLightBuffer->data.lights[i].range, 0, 100); // ����Ʈ ũ��?
			ImGui::ColorEdit4("LightColor", (float*)&mLightBuffer->data.lights[i].color);			// ����Ʈ����
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
	SetPerspectiveProjectionBuffer();

	if (mbIsTargetCamera)
	{
		mTargetCamera->SetVS(1);
	}
	else
	{
		mWorldCamera->SetVS(1);
	}

	mLightBuffer->SetPSBuffer(0);
}

void Environment::SetViewport(UINT width, UINT height)
{
	mViewPort.Width = (float)width;
	mViewPort.Height = (float)height;
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;

	DEVICECONTEXT->RSSetViewports(1, &mViewPort);
}

void Environment::SetTargetToCamera(Transform* target)
{
	this->mTarget = target;
	mTargetCamera->SetTarget(target);
}

void Environment::SetPerspectiveProjectionBuffer()
{
	mPerspectiveProjectionBuffer->SetVSBuffer(2);
}

void Environment::SetOrthographicProjectionBuffer()
{
	mOrthographicProjectionBuffer->SetVSBuffer(2);
}

void Environment::createPerspectiveBuffer()
{
	mPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, 
		WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f); // �þ߰�( pie/4�ϱ� 90��?,

	mPerspectiveProjectionBuffer = new ProjectionBuffer();
	mPerspectiveProjectionBuffer->Set(mPerspectiveProjectionMatrix);
}

void Environment::createOrthographicBuffer()
{
	mOrthographicProjectionMatrix = XMMatrixOrthographicLH(WIN_WIDTH, WIN_HEIGHT, 0.1f, 1000.0f);

	mOrthographicProjectionBuffer = new ProjectionBuffer();
	mOrthographicProjectionBuffer->Set(mOrthographicProjectionMatrix);
}




Vector3 Environment::GetLightPosition()
{
	Vector3 t;
	t.x = mLightBuffer->data.lights[0].position.x;
	t.y = mLightBuffer->data.lights[0].position.y;
	t.z = mLightBuffer->data.lights[0].position.z;

	return t;
}