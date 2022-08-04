#pragma once

class PlayerStatusBar : public UserInterface
{
public:
	PlayerStatusBar();
	~PlayerStatusBar();

	virtual void Hide() override;
	virtual void Show() override;
	virtual void Update() override;
	virtual void Render() override;
	void PostRender();

	void SetMaxHP(float maxHP) { mMaxHP = maxHP; }
	void SetCurHP(float curHP) { mCurHP = curHP; }
	void SetHPRate(float hpRate) { mHPRate = hpRate; }

	void SetMaxMP(float maxMP) { mMaxMP = maxMP; }
	void SetCurMP(float curMP) { mCurMP = curMP; }
	void SetMPRate(float mpRate) { mHPRate = mpRate; }

private:

	
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
	pair<float, float> mPortraitUIPositionOffset;

	float mCurHP;
	float mMaxHP;
	float mHPRate;

	float mCurMP;
	float mMaxMP;
	float mMPRate;
};