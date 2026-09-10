#pragma once

#include <CoreAsset/AssetType.h>
#include <CoreAsset/PakType.h>
#include <CoreBase/ThreadSafeQueue.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
namespace CoreAsset
{
struct IntermediateAsset;
class Asset;
} // namespace CoreAsset

namespace Quad
{
class EditorBuildManager;

}

// Build를 위해 필요한 정보
struct ProjectBuildRequest
{
    std::filesystem::path mOutputDirectory;
    std::filesystem::path mProjectDirectory;
    std::filesystem::path mRuntimeTemplateDirectory;
    std::string mProjectName;
    std::filesystem::path mEngineDirectory;
    CoreAsset::AssetID mStartUpMapID;
    std::vector<PakWriteAssetRecord> mPakWriteAssetRecordList;
};

enum class EProjectBuildEventType
{
    eProgress = 0,
    eCompleted,
    eFailed
};
struct ProjectBuildResult
{
    bool mSuccess = false;
    std::filesystem::path mOutputDirectory;
    int mProcessExitCode = 0;
    std::string mErrorMessage;
};

struct ProjectBuildEvent
{
    EProjectBuildEventType mType;
    float mProgress = 0.0f;
    std::string mMessage;

    ProjectBuildResult mResult;
};

struct ProjectBuildNotifyChannel
{
    ThreadSafeQueue<ProjectBuildEvent> mNofiyQueue;
};

// 호출자가 소유하게된다.(ex 메인스레드 )
struct ProjectBuildTaskHandle
{

  public:
    ProjectBuildTaskHandle() {}

    ~ProjectBuildTaskHandle() = default;

    bool TryPopEvent(ProjectBuildEvent &event)
    {
        return mNotifyChannel && mNotifyChannel->mNofiyQueue.TryPop(event);
    }

  private:
    friend class Quad::EditorBuildManager;
    std::shared_ptr<ProjectBuildNotifyChannel> mNotifyChannel;
};

// 작업자가 소유하게된다 (ex importer 스레드 )
struct ProjectBuildJobContext
{
  public:
    ProjectBuildJobContext(std::shared_ptr<ProjectBuildNotifyChannel> channel) : mNotifyChannel(std::move(channel)) {}
    ~ProjectBuildJobContext() = default;

    // push  -  >reportProgress
    void ReportProgress(float progress, const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ProjectBuildEvent event;
        event.mType = EProjectBuildEventType::eProgress;
        event.mProgress = progress;
        event.mMessage = message;

        mNotifyChannel->mNofiyQueue.Push(event);
    }
    void ReportCompeleted(const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ProjectBuildEvent event;
        event.mType = EProjectBuildEventType::eCompleted;
        event.mMessage = message;
        event.mResult.mSuccess = true;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

    //  report error
    void ReportFailed(const std::string &message)
    {

        if (mNotifyChannel == nullptr)
            return;

        ProjectBuildEvent event;
        event.mType = EProjectBuildEventType::eFailed;
        event.mMessage = message;

        mNotifyChannel->mNofiyQueue.Push(event);
    }

  private:
    std::shared_ptr<ProjectBuildNotifyChannel> mNotifyChannel;
};
