#pragma once

class UserInterface : public Transform// UI�� �ֻ��� Ŭ����.
{

	virtual void Hide() = 0;
	virtual void SetHideDir() = 0;

};