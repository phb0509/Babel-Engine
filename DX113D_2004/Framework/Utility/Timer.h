#pragma once

class Timer : public Singleton<Timer>
{
private:
	friend class Singleton;

	float timeScale;
	float timeElapsed;

	__int64 curTime;
	__int64 lastTime;
	__int64 periodFrequency;

	int frameRate;
	int frameCount;

	float oneSecCount;
	float runTime;

	float lockFPS;

	Timer();
	~Timer();
public:
	void Update();

	void SetLockFPS(float value) { lockFPS = value; }

	int GetFPS() { return frameRate; }
	float GetElapsedTime() { return timeElapsed; }
	float GetRunTime() { return runTime; }
};