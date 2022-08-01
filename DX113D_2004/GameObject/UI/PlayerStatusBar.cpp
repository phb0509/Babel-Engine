#include "Framework.h"


PlayerStatusBar::PlayerStatusBar() :
    mCurHP(1.0f),
    mMaxHP(1.0f),
    mHPRate(1.0f),
    mCurMP(1.0f),
    mMaxMP(1.0f),
    mMPRate(1.0f),
    mScaleOffset(1.0f)
{
	mBackGroundTexture = Texture::Add(L"UI_Resource/Player_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Player_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Player_Status/MP_Bar.png");

	float textureWidth = mBackGroundTexture->GetWidth();
	float textureHeight = mBackGroundTexture->GetHeight();
	mScaleOffset = 4 / 5.0f;

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("PlayerStatusBarBackGround");
	mBackGroundUI->mScale = { textureWidth * mScaleOffset,textureHeight * mScaleOffset ,0.0f };
	
	//mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI = new UIImage(L"Texture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("PlayerHPBar");
	textureWidth = mHPBarTexture->GetWidth();
	textureHeight = mHPBarTexture->GetHeight();
	mHPBarUI->mScale = { textureWidth * mScaleOffset,textureHeight * mScaleOffset ,0.0f };

	//mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI = new UIImage(L"Texture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("PlayerMPBar");
	textureWidth = mMPBarTexture->GetWidth();
	textureHeight = mMPBarTexture->GetHeight();
	mMPBarUI->mScale = { textureWidth * mScaleOffset,textureHeight * mScaleOffset ,0.0f };

	// 고정값
	mStandPosition.x = 417.0f;
	mStandPosition.y = 147.0f;

	// Offset값 적용.

	mStandPosition *= mScaleOffset;
	mBackGroundUI->mPosition = mStandPosition;
	mHPBarUI->mPosition = { mStandPosition.x + 101.2f * mScaleOffset , mStandPosition.y + 42.73f * mScaleOffset, 0.0f };
	mMPBarUI->mPosition = { mStandPosition.x + 52.0f * mScaleOffset, mStandPosition.y - 13.3f * mScaleOffset, 0.0f };
}

PlayerStatusBar::~PlayerStatusBar()
{
	GM->SafeDelete(mBackGroundUI);
	GM->SafeDelete(mHPBarUI);
	GM->SafeDelete(mMPBarUI);
}

void PlayerStatusBar::Update()
{
	mHPRate = mCurHP / mMaxHP;
	mHPBarUI->SetWidthRatio(mHPRate);
	mMPBarUI->SetWidthRatio(mMPRate);

	UpdateWorld();
	mBackGroundUI->Update();
	mHPBarUI->Update();
	mMPBarUI->Update();
}

void PlayerStatusBar::Render()
{
	mHPBarUI->Render();
	mMPBarUI->Render();
	mBackGroundUI->Render();
}

void PlayerStatusBar::PostRender()
{
	mBackGroundUI->PostTransformRender();
	mHPBarUI->PostTransformRender();
	mMPBarUI->PostTransformRender();
}

void PlayerStatusBar::Hide()
{
	
}

void PlayerStatusBar::Show()
{
}


