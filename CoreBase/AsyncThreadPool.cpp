#include "AsyncThreadPool.h"

AsyncThreadPool *AsyncThreadPool::GetInstance()
{
    static AsyncThreadPool instance;

    return &instance;
}

AsyncThreadPool::AsyncThreadPool() : mNextThreadIndex(0)
{

    for (size_t i = 0; i < 10; ++i)
    {
        mThreadList.push_back(std::make_unique<AsyncThread>());
    }
}

AsyncThreadPool::~AsyncThreadPool() {}

void AsyncThreadPool::Submit(std::function<void()> task)
{

    mThreadList[mNextThreadIndex]->Start(task);
    mNextThreadIndex = (mNextThreadIndex + 1) % 10;
}
