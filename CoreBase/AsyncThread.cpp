#include "AsyncThread.h"

AsyncThread::AsyncThread() : mLoopState(true)
{

    mThread = std::thread([this]() { WorkerLoop(); });
}

AsyncThread::~AsyncThread()
{
    mLoopState = false;
    mCV_WorkQueue.notify_all();
    mThread.join();
}

void AsyncThread::SetName(const std::string name)
{
    mName = name;
}

void AsyncThread::Start(const std::function<void()> &task)
{
    std::lock_guard lock(mWorkQueueMutex);

    mWorkQueue.push(task);

    mCV_WorkQueue.notify_all();
}

void AsyncThread::WorkerLoop()
{

    while (1)
    {

        std::function<void()> task;

        {
            std::unique_lock lock(mWorkQueueMutex);

            // 작업이 없고 루프 상태가 false라면 스레드 종료
            if (mWorkQueue.empty() && !mLoopState)
                break;

            // 작업이 존재하거나, or 루프상태가 false이면 깨어난다.
            if (mWorkQueue.empty() && mLoopState)
                mCV_WorkQueue.wait(lock, [this]() { return !mWorkQueue.empty() || !mLoopState; });

            // 작업이 없고 루프 상태가 false라면 스레드 종료
            if (mWorkQueue.empty() && !mLoopState)
                break;

            task = mWorkQueue.front();
            mWorkQueue.pop();
        }

        task();
    }
}
