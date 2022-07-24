#include "Framework.h"
#include "MapToolScene.h"

MapToolScene::MapToolScene()
{
	mWorldCamera = new Camera();
	mWorldCamera->mPosition = { -35.2f,113.6f, -105.7f };
	mWorldCamera->mRotation = { 0.5f,0.4f, 0.0f };
	mWorldCamera->mMoveSpeed = 200.0f;
	mWorldCamera->mWheelSpeed = 30.0f;

	mTerrainEditor = new TerrainEditor();
	mTerrainEditor->SetCamera(mWorldCamera);
	//skyBox = new SkyBox();

	mRasterizerState = new RasterizerState();
	mRasterizerState->FillMode(D3D11_FILL_WIREFRAME);
}

MapToolScene::~MapToolScene()
{
	GM->SafeDelete(mTerrainEditor);
	GM->SafeDelete(mRasterizerState);
	GM->SafeDelete(mWorldCamera);
}

void MapToolScene::Update()
{
	mWorldCamera->Update();
	moveWorldCamera();

	mTerrainEditor->Update();
}

void MapToolScene::PreRender()
{
	Environment::Get()->Set(); // ºä¹öÆÛ Set VS
	mWorldCamera->SetViewBufferToVS();
	mWorldCamera->SetProjectionBufferToVS();

	mTerrainEditor->PreRender();
}

void MapToolScene::Render()
{
	// ¹é¹öÆÛ¿Í ¿¬°áµÈ ·»´õÅ¸°Ù
	Device::Get()->ClearRenderTargetView(Float4(0.18f, 0.18f, 0.25f, 1.0f));
	Device::Get()->ClearDepthStencilView();
	Device::Get()->SetRenderTarget();
	Environment::Get()->Set(); // SetViewPort

	mWorldCamera->SetViewBufferToVS();
	mWorldCamera->SetProjectionBufferToVS();

	//skyBox->Render();
	//mRasterizerState->SetState();
	mTerrainEditor->Render();
}

void MapToolScene::PostRender()
{
	mTerrainEditor->PostRender();
}

void MapToolScene::moveWorldCamera()
{
	// Update Position
	if (KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('I'))
			mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('K'))
			mWorldCamera->mPosition -= mWorldCamera->GetForwardVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('J'))
			mWorldCamera->mPosition -= mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('L'))
			mWorldCamera->mPosition += mWorldCamera->GetRightVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('U'))
			mWorldCamera->mPosition -= mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
		if (KEY_PRESS('O'))
			mWorldCamera->mPosition += mWorldCamera->GetUpVector() * mWorldCamera->mMoveSpeed * DELTA;
	}

	mWorldCamera->mPosition += mWorldCamera->GetForwardVector() * Control::Get()->GetWheel() * mWorldCamera->mWheelSpeed * DELTA;

	// Update Rotation
	if (KEY_PRESS(VK_RBUTTON))
	{
		Vector3 value = MOUSEPOS - mPreFrameMousePosition;

		mWorldCamera->mRotation.x += value.y * mWorldCamera->mRotationSpeed * DELTA;
		mWorldCamera->mRotation.y += value.x * mWorldCamera->mRotationSpeed * DELTA;
	}

	mPreFrameMousePosition = MOUSEPOS;
	mWorldCamera->SetViewMatrixToBuffer();
}
