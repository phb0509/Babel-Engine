#pragma once
#include "Framework/Math/Vector3.h"

#define KEYMAX 255

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

	::byte curState[KEYMAX];
	::byte oldState[KEYMAX];
	::byte mapState[KEYMAX];

	Vector3 mousePos;
	float wheelValue;

	Control();
	~Control();
public:

	void Update();

	bool Down(UINT key) { return mapState[key] == DOWN; }
	bool Up(UINT key) { return mapState[key] == UP; }
	bool Press(UINT key) { return mapState[key] == PRESS; }

	Vector3 GetMouse() { return mousePos; }
	void SetMouse(LPARAM lParam);

	float GetWheel() { return wheelValue; }
	void SetWheel(float value) { wheelValue = value; }
};