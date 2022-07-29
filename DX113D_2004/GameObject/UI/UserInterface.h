#pragma once

class UserInterface : public Transform// UI의 최상위 클래스.
{

public:
	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	void SetHideDirection(Vector3 hideDirection) { mHideDirection = hideDirection; }

protected:
	Vector3 mHideDirection;

};