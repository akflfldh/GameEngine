#pragma once
#include<Windows.h>
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB GameTimer
	{
	public:

		GameTimer();
		~GameTimer() = default;
		void Reset();
		void Tick();	//매프레임호출을 반드시 해야한다.
		float GetDeltaTime() const;
		float TotalTime()const;

		void Start();
		void Stop();
	private:

		double mSecondsPerCount;
		double mDeltaTime;
		__int64 mPrevTime;
		__int64 mBaseTime;
		__int64 mPausedTime;//정지되었던 시간  (전체시간에서 정지되었던시간을 뺴야한다)
		__int64 mStopTime;//정지가 시작됬던 시간

		bool mIsStopped;

	};

}