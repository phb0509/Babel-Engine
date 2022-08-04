#include "Framework.h"


MonsterStatusBar::MonsterStatusBar() :
	mCurHP(1.0f),
	mMaxHP(1.0f),
	mHPRate(1.0f),
	mCurMP(1.0f),
	mMaxMP(1.0f),
	mMPRate(1.0f)
{
	mBackGroundTexture = Texture::Add(L"UI_Resource/Monster_Status/Status_BackGround.png");
	mHPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/HP_Bar.png");
	mMPBarTexture = Texture::Add(L"UI_Resource/Monster_Status/MP_Bar.png");
	mPortraitTexture = Texture::Add(L"UI_Resource/Monster_Status/Default_Portrait.png");
	mPortraitBackGroundTexture = Texture::Add(L"UI_Resource/Monster_Status/Default_Portrait_BackGround.png");

	mBackGroundUI = new UIImage(L"Texture");
	mBackGroundUI->SetSRV(mBackGroundTexture->GetSRV());
	mBackGroundUI->SetTag("Monster_StatusBarBackGround");
	mBackGroundTextureSize = make_pair(mBackGroundTexture->GetWidth(), mBackGroundTexture->GetHeight());
	mBackGroundUI->mScale = { mBackGroundTextureSize.first, mBackGroundTextureSize.second, 0.0f };

	mHPBarUI = new UIImage(L"VariableTexture");
	mHPBarUI->SetSRV(mHPBarTexture->GetSRV());
	mHPBarUI->SetTag("Monster_HPBar");
	mHPBarTextureSize = make_pair(mHPBarTexture->GetWidth(), mHPBarTexture->GetHeight());
	mHPBarUI->mScale = { mHPBarTextureSize.first, mHPBarTextureSize.second, 0.0f };

	mMPBarUI = new UIImage(L"VariableTexture");
	mMPBarUI->SetSRV(mMPBarTexture->GetSRV());
	mMPBarUI->SetTag("Monster_MPBar");
	mMPBarTextureSize = make_pair(mMPBarTexture->GetWidth(), mMPBarTexture->GetHeight());
	mMPBarUI->mScale = { mMPBarTextureSize.first, mMPBarTextureSize.second, 0.0f };

	mPortraitUI = new UIImage(L"Texture");
	mPortraitUI->SetSRV(mPortraitTexture->GetSRV());
	mPortraitUI->SetTag("Mutant_Portrait");
	mPortraitTextureSize = make_pair(mPortraitTexture->GetWidth(), mPortraitTexture->GetHeight());
	mPortraitUI->mScale = { mPortraitTextureSize.first, mPortraitTextureSize.second, 0.0f };

	mPortraitBackGroundUI = new UIImage(L"Texture");
	mPortraitBackGroundUI->SetSRV(mPortraitBackGroundTexture->GetSRV());
	mPortraitBackGroundUI->SetTag("Monster_PortraitBackGround");
	mPortraitBackGroundTextureSize = make_pair(mPortraitBackGroundTexture->GetWidth(), mPortraitBackGroundTexture->GetHeight());
	mPortraitBackGroundUI->mScale = { mPortraitBackGroundTextureSize.first, mPortraitBackGroundTextureSize.second, 0.0f };

	 //Offset
	mHPBarUIPositionOffset = make_pair(230.0f, 133.0f);
	mMPBarUIPositionOffset = make_pair(240.0f, 87.8f);

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
	//mStandScaleOffsetX = this->mScale.x * WIN_WIDTH / MAIN_WIN_WIDTH;
	//mStandScaleOffsetY = this->mScale.y * WIN_HEIGHT / MAIN_WIN_HEIGHT;
	Vector3 standPosition = this->mPosition;
	Vector3 standScale = this->mScale;

	mBackGroundUI->mPosition = standPosition;
	mHPBarUI->mPosition = { standPosition.x + mHPBarUIPositionOffset.first * standScale.x, standPosition.y + mHPBarUIPositionOffset.second * standScale.y, 0.0f };
	mMPBarUI->mPosition = { standPosition.x + mMPBarUIPositionOffset.first * standScale.x, standPosition.y + mMPBarUIPositionOffset.second * standScale.y, 0.0f };
	mPortraitUI->mPosition = { standPosition.x, standPosition.y, 0.0f };
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

void MonsterStatusBar::Render()
{
	mHPBarUI->Render();
	mMPBarUI->Render();
	mPortraitBackGroundUI->Render();
	mPortraitUI->Render();
	mBackGroundUI->Render();
}

void MonsterStatusBar::PostRender()
{
	mBackGroundUI->PostTransformRender();
	mHPBarUI->PostTransformRender();
	mMPBarUI->PostTransformRender();
	mPortraitUI->PostTransformRender();
	mPortraitBackGroundUI->PostTransformRender();
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


