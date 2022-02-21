#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	Environment::Get()->SetIsEnabledTargetCamera(false); // 월드카메라만 사용.

	// 카메라 설정.
	WORLDCAMERA->mPosition = { 0.0f, 0.0f, -40.0f };
	WORLDCAMERA->mRotation = { 0.0, 0.0, 0.0 };
	WORLDCAMERA->mMoveSpeed = 50.0f;

	mBoxCollider = new BoxCollider;
	mBoxCollider->mScale = { 30.0f,30.0f,30.0f };
	mBoxCollider->mPosition = { 0.0f, 0.0f, 20.0f };

	mCube = new Cube();
	mCube->mScale = { 30.0f,30.0f,30.0f };
	mCube->mPosition = { 0.0f,0.0f,0.0f };
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Update();
	}

	Environment::Get()->GetWorldCamera()->Update();

	mCube->Update();
	mBoxCollider->Update();
}

void TestScene::PreRender()
{

}

void TestScene::Render()
{
	Device::Get()->ClearRenderTargetView();
	Device::Get()->SetRenderTarget(); // SetMainRenderTarget
	
	Environment::Get()->SetPerspectiveProjectionBuffer();

	if (Environment::Get()->GetIsEnabledTargetCamera())
	{
		Environment::Get()->GetTargetCamera()->Render();
	}

	Environment::Get()->GetWorldCamera()->Render(); // FrustumRender 외엔 뭐 업승ㅁ.
	Environment::Get()->Set(); // SetViewPort


	//Environment::Get()->SetOrthographicProjectionBuffer();
	
	//Device::Get()->Clear();
	mCube->Render();
	Device::Get()->SetRenderTargetNullDSV();
	mBoxCollider->Render();
}

void TestScene::PostRender()
{

}
