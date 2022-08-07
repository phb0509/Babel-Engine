#pragma once

class MonsterStatusBar : public UserInterface
{
public:
	MonsterStatusBar();
	~MonsterStatusBar();

	virtual void Hide() override;
	virtual void Show() override;
	virtual void Update() override;
	virtual void Render() override;
	void PostRender();

	void SetCurMainCamera(Camera* mainCamera);
	void SetPortraitTexture(Texture* portraitTexture);

private:
	void initialize();

private:
	// Texture
	Texture* mBackGroundTexture;
	Texture* mHPBarTexture;
	Texture* mMPBarTexture;
	Texture* mPortraitTexture;
	Texture* mPortraitBackGroundTexture;

	// UIImage
	UIImage* mBackGroundUI;
	UIImage* mHPBarUI;
	UIImage* mMPBarUI;
	UIImage* mPortraitUI;
	UIImage* mPortraitBackGroundUI;

	// TextureSize
	pair<float, float> mBackGroundTextureSize;
	pair<float, float> mHPBarTextureSize;
	pair<float, float> mMPBarTextureSize;
	pair<float, float> mPortraitTextureSize;
	pair<float, float> mPortraitBackGroundTextureSize;

	// Offset
	pair<float, float> mHPBarUIPositionOffset;
	pair<float, float> mMPBarUIPositionOffset;

	float mCurHP;
	float mMaxHP;
	float mHPRate;

	float mCurMP;
	float mMaxMP;
	float mMPRate;
	float mStandScaleOffsetX;
	float mStandScaleOffsetY;

	Camera* mCurMainCamera;
	bool mbIsInitialize;
};