#pragma once

class UserInterface : public Transform// UI의 최상위 클래스.
{

	virtual void Hide() = 0;
	virtual void SetHideDir() = 0;

};