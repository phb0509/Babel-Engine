#include "Framework.h"
#include "Timer.h"

Timer::Timer()
	: mFrameRate(0), mFrameCount(0), mElapsedTime(0), mOneSecCount(0),
	mRunTime(0), mLockFPS(0)
{	
	//1�ʵ��� CPU�� �������� ��ȯ�ϴ� �Լ�
	QueryPerformanceFrequency((LARGE_INTEGER*)&mPeriodFrequency);

	//���� CPU������
	QueryPerformanceCounter((LARGE_INTEGER*)&mLastTime);

	mTimeScale = 1.0f / (float)mPeriodFrequency;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurTime);
	mElapsedTime = (float)(mCurTime - mLastTime) * mTimeScale;

	if (mLockFPS != 0.0f)
	{
		while (mElapsedTime < (1.0f / mLockFPS))
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&mCurTime);
			mElapsedTime = (float)(mCurTime - mLastTime) * mTimeScale;
		}
	}

	mLastTime = mCurTime;

	//FPS(Frame Per Second)
	mFrameCount++;
	mOneSecCount += mElapsedTime;

	if (mOneSecCount >= 1.0f)
	{
		mFrameRate = mFrameCount;
		mFrameCount = 0;
		mOneSecCount = 0.0f;
	}

	mRunTime += mElapsedTime;
}
