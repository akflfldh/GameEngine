#pragma once

#include <mutex>
#include <queue>

template <typename T> class ThreadSafeQueue
{
  public:
    ThreadSafeQueue();
    ~ThreadSafeQueue();

    ThreadSafeQueue(const ThreadSafeQueue &) = delete;
    ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;
    // Push

    void Push(T value);

    // Pop
    bool TryPop(T &out);

    // Empty
    bool Empty() const;

  private:
    std::queue<T> mQueue;
    std::mutex mMutex;
};
template <typename T> inline ThreadSafeQueue<T>::ThreadSafeQueue() {}

template <typename T> inline ThreadSafeQueue<T>::~ThreadSafeQueue() {}

template <typename T> inline void ThreadSafeQueue<T>::Push(T value)
{

    std::lock_guard<std::mutex> lock(mMutex);
    mQueue.push(std::move(value));
}

template <typename T> inline bool ThreadSafeQueue<T>::TryPop(T &out)
{

    std::lock_guard<std::mutex> lock(mMutex);

    if (mQueue.empty())
        return false;

    out = mQueue.front();
    mQueue.pop();

    return true;
}

template <typename T> inline bool ThreadSafeQueue<T>::Empty() const
{

    std::lock_guard<std::mutex> lock(mMutex);

    if (mQueue.empty())
        return true;

    return false;
}
