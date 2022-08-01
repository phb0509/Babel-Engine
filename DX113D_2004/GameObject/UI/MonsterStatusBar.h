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

	void SetCamera(Camera* camera);
	void SetPortraitTexture(Texture* portraitTexture) { mPortraitTexture = portraitTexture; }

private:
	Texture* mBackGroundTexture;
	Texture* mHPBarTexture;
	Texture* mMPBarTexture;
	Texture* mPortraitTexture;

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
	float mStandScaleOffset;
	Camera* mCamera;
};