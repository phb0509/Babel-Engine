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

private:
	UIImage* mBackGround;
	UIImage* mHPBar;
	UIImage* mMPBar;

	float mCurrentHP;
	float mMaxHP;
};