#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	float mAspectRatio = WIN_WIDTH / (float)WIN_HEIGHT;
	float mFoV = XM_PIDIV4;
	float mDistanceToNearZ = 10.0f;
	float mDistanceToFarZ = 100.0f;

	float nearHeight = 2 * tan(mFoV / 2.0f) * mDistanceToNearZ;
	float nearWidth = nearHeight * mAspectRatio;

	float farHeight = 2 * tan(mFoV / 2.0f) * mDistanceToFarZ;
	float farWidth = farHeight * mAspectRatio;


	mFrustumCollider = new FrustumCollider(nearWidth, nearHeight, farWidth, farHeight, mDistanceToNearZ, mDistanceToFarZ);
	
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{
	mFrustumCollider->Update();
}

void TestScene::PreRender()
{
}

void TestScene::Render()
{
	mFrustumCollider->Render();
}

void TestScene::PostRender()
{
}
