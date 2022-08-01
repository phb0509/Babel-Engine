#include "Framework.h"


MonsterStatusBar::MonsterStatusBar() :
	mCurHP(1.0f),
	mMaxHP(1.0f),
	mHPRate(1.0f),
	mCurMP(1.0f),
	mMaxMP(1.0f),
	mMPRate(1.0f),
	mStandScaleOffset(1.0f)
{
	mBackGroundTexture = Texture::Add(L"UI_Resource/Monster_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/MP_Bar.png");

	float textureWidth = mBackGroundTexture->GetWidth();
	float textureHeight = mBackGroundTexture->GetHeight();
	mStandScaleOffset = 3 / 4.0f; // 기준윈도우크기(1600 * 900) 기준.

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("PlayerStatusBarBackGround");
	mBackGroundUI->mScale = { textureWidth * mStandScaleOffset,textureHeight * mStandScaleOffset ,0.0f };

	//mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("PlayerHPBar");
	textureWidth = mHPBarTexture->GetWidth();
	textureHeight = mHPBarTexture->GetHeight();
	mHPBarUI->mScale = { textureWidth * mStandScaleOffset,textureHeight * mStandScaleOffset ,0.0f };

	//mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("PlayerMPBar");
	textureWidth = mMPBarTexture->GetWidth();
	textureHeight = mMPBarTexture->GetHeight();
	mMPBarUI->mScale = { textureWidth * mStandScaleOffset,textureHeight * mStandScaleOffset ,0.0f };

	// 고정값
	mStandPosition.x = 417.0f;
	mStandPosition.y = 147.0f;

	// Offset값 적용.

	mStandPosition *= mStandScaleOffset;
	mBackGroundUI->mPosition = mStandPosition;
	mHPBarUI->mPosition = { mStandPosition.x + 101.2f * mStandScaleOffset , mStandPosition.y + 42.73f * mStandScaleOffset, 0.0f };
	mMPBarUI->mPosition = { mStandPosition.x + 52.0f * mStandScaleOffset, mStandPosition.y - 13.3f * mStandScaleOffset, 0.0f };
}

MonsterStatusBar::~MonsterStatusBar()
{
	GM->SafeDelete(mBackGroundUI);
	GM->SafeDelete(mHPBarUI);
	GM->SafeDelete(mMPBarUI);
}

void MonsterStatusBar::Update()
{
	mHPRate = mCurHP / mMaxHP;
	mHPBarUI->SetWidthRatio(mHPRate);
	mMPBarUI->SetWidthRatio(mMPRate);

	UpdateWorld();
	mBackGroundUI->Update();
	mHPBarUI->Update();
	mMPBarUI->Update();
}

void MonsterStatusBar::Render()
{
	mHPBarUI->Render();
	mMPBarUI->Render();
	mBackGroundUI->Render();
}

void MonsterStatusBar::PostRender()
{
	mBackGroundUI->PostTransformRender();
	mHPBarUI->PostTransformRender();
	mMPBarUI->PostTransformRender();
}

void MonsterStatusBar::SetCamera(Camera* camera)
{
	mBackGroundUI->SetCamera(camera);
	mHPBarUI->SetCamera(camera);
	mMPBarUI->SetCamera(camera);
}

void MonsterStatusBar::Hide()
{

}

void MonsterStatusBar::Show()
{
}


