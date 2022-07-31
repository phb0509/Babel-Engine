#include "Framework.h"


PlayerStatusBar::PlayerStatusBar():
   mCurHP(1.0f),
   mMaxHP(1.0f),
   mHPRate(1.0f),
   mCurMP(1.0f),
   mMaxMP(1.0f),
   mMPRate(1.0f)
{
	mBackGroundTexture = Texture::Add(L"UI_Resource/Player_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Player_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Player_Status/MP_Bar.png");

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("PlayerStatusBarBackGround");
	mBackGroundUI->mScale = { 814.0f,253.0f,0.0f };
	
	//mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI = new UIImage(L"Texture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("PlayerHPBar");
	//mHPBarUI->SetParent(&mBackGroundUI->GetWorldMatrixValue());
	mHPBarUI->mPosition = { 50.0f,50.0f,0.0f };
	mHPBarUI->mScale = { 426.0f,42.0f,0.0f };

	//mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI = new UIImage(L"Texture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("PlayerMPBar");
	//mMPBarUI->SetParent(&mBackGroundUI->GetWorldMatrixValue());
	mMPBarUI->mPosition = { 50.0f,100.0f,0.0f };
	mMPBarUI->mScale = { 330.0f,26.0f,0.0f };
}

PlayerStatusBar::~PlayerStatusBar()
{
	GM->SafeDelete(mBackGroundUI);
	GM->SafeDelete(mHPBarUI);
	GM->SafeDelete(mMPBarUI);
}

void PlayerStatusBar::Update()
{
	mBackGroundUI->mPosition = this->mPosition;
	//mHPBarUI->mPosition = { 50.0f,50.0f,0.0f };
	//mMPBarUI->mPosition = { 50.0f,100.0f,0.0f };

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
	mBackGroundUI->Render();
	mHPBarUI->Render();
	mMPBarUI->Render();
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


