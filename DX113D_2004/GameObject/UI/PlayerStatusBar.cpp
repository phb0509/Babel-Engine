#include "Framework.h"


PlayerStatusBar::PlayerStatusBar():
	mCurrentHP(0.0f),
	mMaxHP(0.0f)
{
	//mTargetTextures[i] = new UIImage(L"Texture"); //UIImage �迭.
	//mTargetTextures[i]->SetSRV(mShowSRVs[i]); // ��� srv(�̹���)
	//mTargetTextures[i]->mScale = { 200.0f, 200.0f, 200.0f };
	//mExtensionPreviewImages["png"] = Texture::Add(L"Textures/tempPreviewImage.jpg");

	mBackGround = new UIImage(L"Texture");
	mHPBar = new UIImage(L"Texture");
	mMPBar = new UIImage(L"Texture");

	mBackGround->SetTag("PlayerStatusBarBackGround");
	mHPBar->SetTag("PlayerHPBar");
	mMPBar->SetTag("PlayerMPBar");
}

PlayerStatusBar::~PlayerStatusBar()
{
	GM->SafeDelete(mBackGround);
	GM->SafeDelete(mHPBar);
	GM->SafeDelete(mMPBar);
}

void PlayerStatusBar::Hide()
{
	
}

void PlayerStatusBar::Show()
{
}

void PlayerStatusBar::Update()
{
	UpdateWorld();
}

void PlayerStatusBar::Render()
{
}
