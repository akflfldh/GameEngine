#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <CoreBase/AsyncThread.h>
#include <CoreBase/CoreBaseDllMacro.h>

// 스레드 풀또한 스레드에 안전하게 만들어야한다 .

class COREBASE_API AsyncThreadPool
{

  public:
    static AsyncThreadPool *GetInstance();
    AsyncThreadPool();
    ~AsyncThreadPool();

    AsyncThreadPool(const AsyncThreadPool &) = delete;
    AsyncThreadPool &operator=(const AsyncThreadPool &) = delete;

    void Submit(std::function<void()> task);

  private:
    std::vector<std::unique_ptr<AsyncThread>> mThreadList;
    size_t mNextThreadIndex;
};
