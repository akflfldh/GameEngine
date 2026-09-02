#pragma once

#include <CoreBase/CoreBaseDllMacro.h>
#include <MainTimer.h>
#include <stdint.h>
#include <vector>

using TimerHandle = uint32_t;

// TODO 논리적타이머 가벼운자료구조
struct Timer
{
};

class COREBASE_API TimerManager
{
  public:
    static TimerManager *GetInstance();
    ~TimerManager();

    MainTimer &GetMainTimer();

  private:
    TimerManager();
    MainTimer mMainTimer;
    std::vector<Timer> mTimerList;
};
