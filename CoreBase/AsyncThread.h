#pragma once

#include <CoreBase/CoreBaseDllMacro.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class COREBASE_API AsyncThread
{
  public:
    AsyncThread();
    virtual ~AsyncThread();

    AsyncThread(const AsyncThread &) = delete;
    AsyncThread &operator=(const AsyncThread &) = delete;

    AsyncThread(AsyncThread &&) = delete;
    AsyncThread &operator=(AsyncThread &&) = delete;

    void SetName(const std::string name);
    virtual void Start(const std::function<void()> &task);

  private:
    void WorkerLoop();

  private:
    std::thread mThread;
    std::mutex mWorkQueueMutex;
    std::string mName;

    std::queue<std::function<void()>> mWorkQueue;
    std::condition_variable mCV_WorkQueue;

    std::atomic<bool> mLoopState;
};
