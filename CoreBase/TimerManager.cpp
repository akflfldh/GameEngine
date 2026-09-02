#include "TimerManager.h"

TimerManager *TimerManager::GetInstance()
{
    static TimerManager instance;
    return &instance;
}

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

MainTimer &TimerManager::GetMainTimer()
{
    return mMainTimer;
    // TODO: 여기에 return 문을 삽입합니다.
}
