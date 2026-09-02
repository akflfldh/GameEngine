#pragma once

#include <CoreBase/ThreadSafeQueue.h>
#include <memory>
#include <string>
#include <vector>

namespace CoreAsset
{
struct IntermediateAsset;
class Asset;
} // namespace CoreAsset

enum class EImportEventType
{
    eProgress = 0,
    eCompleted,
    eFailed
};

struct ImportEvent
{
    EImportEventType mType;
    float mProgress = 0.0f;
    std::string mMessage;

    std::vector<CoreAsset::Asset *> mResults;
};

struct ImportNotifyChannel
{
    ThreadSafeQueue<ImportEvent> mNofiyQueue;
};

// 호출자가 소유하게된다.(ex 메인스레드 )
struct ImportTaskHandle
{

  public:
    ~ImportTaskHandle() = default;

    bool TryPopEvent(ImportEvent &event)
    {
        return mNotifyChannel && mNotifyChannel->mNofiyQueue.TryPop(event);
    }

  private:
    ImportTaskHandle(std::shared_ptr<ImportNotifyChannel> channel) : mNotifyChannel(std::move(channel)) {}

    std::shared_ptr<ImportNotifyChannel> mNotifyChannel;
};

// 작업자가 소유하게된다 (ex importer 스레드 )
struct ImportJobContext
{
  public:
    ImportJobContext(std::shared_ptr<ImportNotifyChannel> channel) : mNotifyChannel(std::move(channel)) {}
    ~ImportJobContext() = default;

    // push  -  >reportProgress
    void ReportProgress(float progress, const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ImportEvent event;
        event.mType = EImportEventType::eProgress;
        event.mProgress = progress;
        event.mMessage = message;

        mNotifyChannel->mNofiyQueue.Push(event);
    }
    void ReportCompeleted(const std::string &message, std::vector<CoreAsset::Asset *> &&asset)
    {

        if (mNotifyChannel == nullptr)
            return;

        ImportEvent event;
        event.mType = EImportEventType::eCompleted;
        event.mMessage = message;
        event.mResults = std::move(asset);

        mNotifyChannel->mNofiyQueue.Push(event);
    }

    // report compeleted
    void ReportCompeleted(const std::string &message, std::vector<CoreAsset::Asset *> asset)
    {

        if (mNotifyChannel == nullptr)
            return;

        ImportEvent event;
        event.mType = EImportEventType::eCompleted;
        event.mMessage = message;
        event.mResults = std::move(asset);

        mNotifyChannel->mNofiyQueue.Push(event);
    }

    //  report error
    void ReportFailed(const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ImportEvent event;
        event.mType = EImportEventType::eFailed;
        event.mMessage = message;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

  private:
    std::shared_ptr<ImportNotifyChannel> mNotifyChannel;
};
