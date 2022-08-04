#include "Framework.h"


PlayerStatusBar::PlayerStatusBar() :
	mCurHP(1.0f),
	mMaxHP(1.0f),
	mHPRate(1.0f),
	mCurMP(1.0f),
	mMaxMP(1.0f),
	mMPRate(1.0f),
	mStandScaleOffsetX(1.0f),
	mStandScaleOffsetY(1.0f)
{
	mBackGroundTexture = Texture::Add(L"UI_Resource/Player_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Player_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Player_Status/MP_Bar.png");
	mPortraitTexture = Texture::Add(L"UI_Resource/Player_Status/Default_Portrait.png");
	mPortraitBackGroundTexture = Texture::Add(L"UI_Resource/Player_Status/Default_Portrait_BackGround.png");

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("PlayerStatusBarBackGround");
	mBackGroundTextureSize = make_pair(mBackGroundTexture->GetWidth(), mBackGroundTexture->GetHeight());
	mBackGroundUI->mScale = { mBackGroundTextureSize.first, mBackGroundTextureSize.second, 0.0f };

	mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("PlayerHPBar");
	mHPBarTextureSize = make_pair(mHPBarTexture->GetWidth(), mHPBarTexture->GetHeight());
	mHPBarUI->mScale = { mHPBarTextureSize.first, mHPBarTextureSize.second, 0.0f };

	mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("PlayerMPBar");
	mMPBarTextureSize = make_pair(mMPBarTexture->GetWidth(), mMPBarTexture->GetHeight());
	mMPBarUI->mScale = { mMPBarTextureSize.first, mMPBarTextureSize.second, 0.0f };

	mPortraitUI = new UIImage(L"Texture");
	mPortraitUI->SetSRV(mPortraitTexture->GetSRV());
	mPortraitUI->SetTag("PortraitUI");
	mPortraitTextureSize = make_pair(mPortraitTexture->GetWidth(), mPortraitTexture->GetHeight());
	mPortraitUI->mScale = { mPortraitTextureSize.first, mPortraitTextureSize.second, 0.0f };

	mPortraitBackGroundUI = new UIImage(L"Texture");
	mPortraitBackGroundUI->SetSRV(mPortraitBackGroundTexture->GetSRV());
	mPortraitBackGroundUI->SetTag("PortraitBackGroundUI");
	mPortraitBackGroundTextureSize = make_pair(mPortraitBackGroundTexture->GetWidth(), mPortraitBackGroundTexture->GetHeight());
	mPortraitBackGroundUI->mScale = { mPortraitBackGroundTextureSize.first, mPortraitBackGroundTextureSize.second, 0.0f };
}



PlayerStatusBar::~PlayerStatusBar()
{
	GM->SafeDelete(mHPBarUI);
	GM->SafeDelete(mMPBarUI);
	GM->SafeDelete(mPortraitBackGroundUI);
	GM->SafeDelete(mPortraitUI);
	GM->SafeDelete(mBackGroundUI);
}

void PlayerStatusBar::Update()
{
	//mStandScaleOffsetX = this->mScale.x * WIN_WIDTH / MAIN_WIN_WIDTH;
	//mStandScaleOffsetY = this->mScale.y * WIN_HEIGHT / MAIN_WIN_HEIGHT;
	Vector3 standPosition = this->mPosition;
	Vector3 standScale = this->mScale;

	mBackGroundUI->mPosition = standPosition;
	mHPBarUI->mPosition = { standPosition.x + 294.737f * standScale.x, standPosition.y + 147.368f * standScale.y, 0.0f };
	mMPBarUI->mPosition = { standPosition.x + 307.0f * standScale.x, standPosition.y + 100.465f * standScale.y, 0.0f };
	mPortraitUI->mPosition = { standPosition.x + 70.0f * standScale.x, standPosition.y, 0.0f };
	mPortraitBackGroundUI->mPosition = mPortraitUI->mPosition;

	
	mBackGroundUI->mScale = { mBackGroundTextureSize.first * standScale.x, mBackGroundTextureSize.second * standScale.y, 0.0f };
	mHPBarUI->mScale = { mHPBarTextureSize.first * standScale.x, mHPBarTextureSize.second * standScale.y, 0.0f };
	mMPBarUI->mScale = { mMPBarTextureSize.first * standScale.x, mMPBarTextureSize.second * standScale.y, 0.0f };
	mPortraitUI->mScale = { mPortraitTextureSize.first * standScale.x, mPortraitTextureSize.second * standScale.y, 0.0f };
	mPortraitBackGroundUI->mScale = { mPortraitBackGroundTextureSize.first * standScale.x, mPortraitBackGroundTextureSize.second * standScale.y, 0.0f };

	mHPRate = mCurHP / mMaxHP;
	mHPBarUI->SetWidthRatio(mHPRate);
	mMPBarUI->SetWidthRatio(mMPRate);

	UpdateWorld();

	mHPBarUI->Update();
	mMPBarUI->Update();
	mPortraitBackGroundUI->Update();
	mPortraitUI->Update();
	mBackGroundUI->Update();
}

void PlayerStatusBar::Render()
{
	mHPBarUI->Render();
	mMPBarUI->Render();
	mPortraitBackGroundUI->Render();
	mPortraitUI->Render();
	mBackGroundUI->Render();
}

void PlayerStatusBar::PostRender()
{
	mBackGroundUI->PostTransformRender();
	mHPBarUI->PostTransformRender();
	mMPBarUI->PostTransformRender();
	mPortraitUI->PostTransformRender();
	mPortraitBackGroundUI->PostTransformRender();
}

void PlayerStatusBar::Hide()
{
}

void PlayerStatusBar::Show()
{
}


