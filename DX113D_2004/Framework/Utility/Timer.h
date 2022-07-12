#pragma once

class Timer : public Singleton<Timer>
{
private:
	friend class Singleton;

	Timer();
	~Timer();

public:
	void Update();

	int GetFPS() { return mFrameRate; }
	float GetElapsedTime() { return mElapsedTime; }
	float GetRunTime() { return mRunTime; }

	void SetLockFPS(float value) { mLockFPS = value; }

private:
	float mTimeScale;
	float mElapsedTime;

	__int64 mCurTime;
	__int64 mLastTime;
	__int64 mPeriodFrequency;

	int mFrameRate;
	int mFrameCount;

	float mOneSecCount;
	float mRunTime;

	float mLockFPS;
};