#include "GameTimer.h"

namespace Quad
{

	GameTimer::GameTimer()
	{
		__int64 countspersecond;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countspersecond);
		mSecondsPerCount = 1.0 / (double)countspersecond;

	}

	void GameTimer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mBaseTime = currTime;
		mPrevTime = currTime;
		mPausedTime = 0;
		mStopTime = 0;
		mIsStopped = false;
	}

	void GameTimer::Tick()
	{
		if (mIsStopped)
		{
			mDeltaTime = 0.0f;
			return;
		}
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mDeltaTime = (currTime - mPrevTime) * mSecondsPerCount;

		mPrevTime = currTime;
		//음수가 되지않도록한다. sdk문서화의 cdxuttimer의 항목에 따르면
		//프로세서가 절전모드로 들어가거나 실행이 다른 프로세서와 
		//엉키는경우 deltaTime이 음수가될수있다.
		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}

	}

	float GameTimer::GetDeltaTime() const
	{
		return (float)mDeltaTime;		//double을리턴해도될텐데 float으로리턴하는이유는?
	}

	float GameTimer::TotalTime() const
	{

		if (mIsStopped)
		{
			return (mStopTime - mBaseTime - mPausedTime) * (float)mSecondsPerCount;
		}
		else
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
			return	(currTime - mBaseTime - mPausedTime) * (float)mSecondsPerCount;
		}


	}

	void GameTimer::Start()
	{
		if (!mIsStopped)
			return;

		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mIsStopped = false;
	}

	void GameTimer::Stop()
	{
		if (mIsStopped)
			return;
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mStopTime = currTime;
		mIsStopped = true;

	}


}