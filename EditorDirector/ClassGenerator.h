#pragma once

#include <CoreBase/ThreadSafeQueue.h>
#include <memory>
#include <string>

class ClassGenerator;

enum class EClassGenerationEventType
{
    eProgress = 0,
    eCompleted,
    eFailed
};

struct ClassGenerationEvent
{
    EClassGenerationEventType mType;
    float mProgress = 0.0f;
    std::string mMessage;
};

struct ClassGenerationNotifyChannel
{
    ThreadSafeQueue<ClassGenerationEvent> mNofiyQueue;
};

struct ClassGenerationTaskHandle
{

  public:
    ClassGenerationTaskHandle() = default;
    ~ClassGenerationTaskHandle() = default;

    bool TryPop(ClassGenerationEvent &event)
    {
        return mNotifyChannel && mNotifyChannel->mNofiyQueue.TryPop(event);
    }

  private:
    friend class ClassGenerator;
    std::shared_ptr<ClassGenerationNotifyChannel> mNotifyChannel;
};

struct ClassGenerationJobContext
{
  public:
    ClassGenerationJobContext(std::shared_ptr<ClassGenerationNotifyChannel> channel) : mNotifyChannel(channel) {}
    ~ClassGenerationJobContext() = default;

    void ReportProgress(float progress, const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ClassGenerationEvent event;
        event.mMessage = message;
        event.mProgress = progress;
        event.mType = EClassGenerationEventType::eProgress;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

    void ReportCompleted(const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ClassGenerationEvent event;
        event.mMessage = message;
        event.mType = EClassGenerationEventType::eCompleted;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

    void ReportFailed(const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ClassGenerationEvent event;
        event.mMessage = message;
        event.mType = EClassGenerationEventType::eFailed;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

  private:
    std::shared_ptr<ClassGenerationNotifyChannel> mNotifyChannel;
};

class ClassGenerator
{
  public:
    static ClassGenerator *GetInstance();

    ClassGenerator();
    ~ClassGenerator();

    ClassGenerationTaskHandle RequestClassGenerate(const std::string &className, const std::string &parentClassName,
                                                   bool isCoreClass = false);

  private:
    void GenerateClass(const std::string &className, const std::string &parentClassName, bool isCoreClass,
                       ClassGenerationJobContext jobContext);
};
