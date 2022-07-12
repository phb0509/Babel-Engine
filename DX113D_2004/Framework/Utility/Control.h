#pragma once
#include "Framework/Math/Vector3.h"

const int KEYMAX = 255;

class Control : public Singleton<Control>
{
private:
	friend class Singleton;

	enum {
		NONE,
		DOWN,
		UP,
		PRESS
	};

	Control();
	~Control();

public:

	void Update();

	bool Down(UINT key) { return mapState[key] == DOWN; }
	bool Up(UINT key) { return mapState[key] == UP; }
	bool Press(UINT key) { return mapState[key] == PRESS; }

	Vector3 GetMouse() { return mousePos; }
	float GetWheel() { return wheelValue; }

	void SetMouse(LPARAM lParam);
	void SetWheel(float value) { wheelValue = value; }

private:
	::byte curState[KEYMAX];
	::byte oldState[KEYMAX];
	::byte mapState[KEYMAX];

	Vector3 mousePos;
	float wheelValue;

};