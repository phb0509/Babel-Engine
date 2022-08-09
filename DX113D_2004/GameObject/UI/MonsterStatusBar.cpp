#include "Framework.h"


MonsterStatusBar::MonsterStatusBar() :
	mCurHP(1.0f),
	mMaxHP(1.0f),
	mHPRate(1.0f),
	mCurMP(1.0f),
	mMaxMP(1.0f),
	mMPRate(1.0f)
{
	this->SetTag("MonsterStatusBar");
	mBackGroundTexture = Texture::Add(L"UI_Resource/Monster_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/MP_Bar.png");
	mPortraitTexture = Texture::Add(L"UI_Resource/Monster_Status/Default_Portrait.png");
	mPortraitBackGroundTexture = Texture::Add(L"UI_Resource/Monster_Status/Default_Portrait_BackGround.png");

	float scaleOffset = 1 / 1.0f;

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("Monster_StatusBarBackGround");
	mBackGroundTextureSize = make_pair(mBackGroundTexture->GetWidth() * scaleOffset, mBackGroundTexture->GetHeight() * scaleOffset);
	mBackGroundUI->mScale = { mBackGroundTextureSize.first, mBackGroundTextureSize.second, 0.0f };

	mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("Monster_HPBar");
	mHPBarTextureSize = make_pair(mHPBarTexture->GetWidth() * scaleOffset, mHPBarTexture->GetHeight() * scaleOffset);
	mHPBarUI->mScale = { mHPBarTextureSize.first, mHPBarTextureSize.second, 0.0f };

	mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("Monster_MPBar");
	mMPBarTextureSize = make_pair(mMPBarTexture->GetWidth() * scaleOffset, mMPBarTexture->GetHeight() * scaleOffset);
	mMPBarUI->mScale = { mMPBarTextureSize.first, mMPBarTextureSize.second, 0.0f };

	mPortraitUI = new UIImage(L"Texture");
	mPortraitUI->SetSRV(mPortraitTexture->GetSRV());
	mPortraitUI->SetTag("Mutant_Portrait");
	mPortraitTextureSize = make_pair(mPortraitTexture->GetWidth() * scaleOffset, mPortraitTexture->GetHeight() * scaleOffset);
	mPortraitUI->mScale = { mPortraitTextureSize.first, mPortraitTextureSize.second, 0.0f };

	mPortraitBackGroundUI = new UIImage(L"Texture");
	mPortraitBackGroundUI->SetSRV(mPortraitBackGroundTexture->GetSRV());
	mPortraitBackGroundUI->SetTag("Monster_PortraitBackGround");
	mPortraitBackGroundTextureSize = make_pair(mPortraitBackGroundTexture->GetWidth() * scaleOffset, mPortraitBackGroundTexture->GetHeight() * scaleOffset);
	mPortraitBackGroundUI->mScale = { mPortraitBackGroundTextureSize.first, mPortraitBackGroundTextureSize.second, 0.0f };

	 //Offset
	mHPBarUIPositionOffset = make_pair(230.0f * scaleOffset, 133.0f * scaleOffset);
	mMPBarUIPositionOffset = make_pair(240.0f * scaleOffset, 87.8f * scaleOffset);

	this->mScale *= 0.4f;
}

MonsterStatusBar::~MonsterStatusBar()
{
	GM->SafeDelete(mHPBarUI);
	GM->SafeDelete(mMPBarUI);
	GM->SafeDelete(mPortraitBackGroundUI);
	GM->SafeDelete(mPortraitUI);
	GM->SafeDelete(mBackGroundUI);
}

void MonsterStatusBar::Update()
{
	Vector3 standPosition = this->mPosition;
	Vector3 standScale = this->mScale;

	mBackGroundUI->mPosition = standPosition;
	mHPBarUI->mPosition = { standPosition.x + mHPBarUIPositionOffset.first * standScale.x, standPosition.y + mHPBarUIPositionOffset.second * standScale.y, standPosition.z };
	mMPBarUI->mPosition = { standPosition.x + mMPBarUIPositionOffset.first * standScale.x, standPosition.y + mMPBarUIPositionOffset.second * standScale.y, standPosition.z };
	mPortraitUI->mPosition = { standPosition.x, standPosition.y, standPosition.z };
	mPortraitBackGroundUI->mPosition = mPortraitUI->mPosition;

	mBackGroundUI->mScale = { mBackGroundTextureSize.first * standScale.x, mBackGroundTextureSize.second * standScale.y, 0.0f };
	mHPBarUI->mScale = { mHPBarTextureSize.first * standScale.x, mHPBarTextureSize.second * standScale.y, 0.0f };
	mMPBarUI->mScale = { mMPBarTextureSize.first * standScale.x, mMPBarTextureSize.second * standScale.y, 0.0f };
	mPortraitUI->mScale = { mPortraitTextureSize.first * standScale.x, mPortraitTextureSize.second * standScale.y, 0.0f };
	mPortraitBackGroundUI->mScale = { mPortraitBackGroundTextureSize.first * standScale.x, mPortraitBackGroundTextureSize.second * standScale.y, 0.0f };

	mHPBarUI->SetWidthRatio(mHPRate);
	mMPBarUI->SetWidthRatio(mMPRate);

	UpdateWorld();

	mHPBarUI->Update();
	mMPBarUI->Update();
	mPortraitBackGroundUI->Update();
	mPortraitUI->Update();
	mBackGroundUI->Update();
}

void MonsterStatusBar::Render()
{
	mHPBarUI->RenderTargetUI();
	mMPBarUI->RenderTargetUI();
	mPortraitBackGroundUI->RenderTargetUI();
	mPortraitUI->RenderTargetUI();
	mBackGroundUI->RenderTargetUI();
}

void MonsterStatusBar::PostRender()
{
	mBackGroundUI->PostTransformRender();
	mHPBarUI->PostTransformRender();
	mMPBarUI->PostTransformRender();
	mPortraitUI->PostTransformRender();
	mPortraitBackGroundUI->PostTransformRender();
}

void MonsterStatusBar::SetCurMainCamera(Camera* camera)
{
	mCurMainCamera = camera;
	mHPBarUI->SetCurMainCamera(camera);
	mMPBarUI->SetCurMainCamera(camera);
	mPortraitBackGroundUI->SetCurMainCamera(camera);
	mPortraitUI->SetCurMainCamera(camera);
	mBackGroundUI->SetCurMainCamera(camera);
}

void MonsterStatusBar::SetPortraitTexture(Texture* portraitTexture)
{
	mPortraitTexture = portraitTexture; 
	mPortraitUI->SetSRV(mPortraitTexture->GetSRV());
}

void MonsterStatusBar::SetHPRate(float hpRate)
{
	mHPRate = hpRate;
	mHPBarUI->SetWidthRatio(mHPRate);
}

void MonsterStatusBar::Hide()
{

}

void MonsterStatusBar::Show()
{
}


