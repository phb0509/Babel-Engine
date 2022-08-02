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
	Texture* mBackGroundTexture;
	Texture* mHPBarTexture;
	Texture* mMPBarTexture;
	Texture* mPortraitTexture;
	Texture* mPortraitBackGroundTexture;

	UIImage* mBackGroundUI;
	UIImage* mHPBarUI;
	UIImage* mMPBarUI;
	UIImage* mPortraitUI;
	UIImage* mPortraitBackGroundUI;

	float mCurHP;
	float mMaxHP;
	float mHPRate;

	float mCurMP;
	float mMaxMP;
	float mMPRate;
	float mStandScaleOffset;

	
	Vector3 testPosition;

	Texture* mMonsterBackGroundTexture;
	Texture* mMonsterHPBarTexture;
	Texture* mMonsterMPBarTexture;
	Texture* mMonsterPortraitTexture;
	Texture* mMonsterPortraitBackGroundTexture;
			  
	UIImage* mMonsterBackGroundUI;
	UIImage* mMonsterHPBarUI;
	UIImage* mMonsterMPBarUI;
	UIImage* mMonsterPortraitUI;
	UIImage* mMonsterPortraitBackGroundUI;
};