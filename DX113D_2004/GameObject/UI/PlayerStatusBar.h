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

private:
	Texture* mBackGroundTexture;
	Texture* mHPBarTexture;
	Texture* mMPBarTexture;

	UIImage* mBackGroundUI;
	UIImage* mHPBarUI;
	UIImage* mMPBarUI;

	float mCurHP;
	float mMaxHP;
	float mHPRate;

	float mCurMP;
	float mMaxMP;
	float mMPRate;

	Vector3 mStandPosition;
	float mScaleOffset;
};