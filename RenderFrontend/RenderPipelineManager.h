#pragma once

#include <CoreDevice/CoreDeviceHeader.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderFrontend/RenderThread.h>
#include <RenderSystem/RenderType.h>
#include <UiSystem/UIType.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Core
{
struct GlobalFrameData;
class LogicalWindow;
}; // namespace Core

class World;

namespace Render
{
class RenderPipeline;
class RenderPassGraph;
struct RenderPassSetUpData;

using PipelineTable =
    std::unordered_map<RenderChannelID, std::unordered_map<ERenderPipelineTag, std::unique_ptr<RenderPipeline>>>;

class RENDER_FRONTEND_API RenderResourcePool
{

  public:
    RenderResourcePool();
    ~RenderResourcePool();

    void Update(uint64_t currentFrame, uint64_t completedFenceValue);

    PooledRenderResource *Alloc(const RenderResourceDesc &desc);
    void Free(PooledRenderResource *resource);

    void DestoryPool();

  private:
    PooledRenderResource *Create(const RenderResourceDesc &desc);
    const uint64_t mResourceExpirationFrame = 60;

    std::vector<PooledRenderResource *> mResourceVec;
    std::mutex mMutex;
};

struct RENDER_FRONTEND_API RenderContext
{
    std::unique_ptr<RenderPassGraph> mRenderPassGraph;
    RenderPassExecuteContext mRenderPassExecuteContext;

    void Reset();
};

struct RenderContextSet
{
    std::vector<RenderContext *> renderContextList;
    std::vector<UI::UIVertex> mUIVertexBuffer;
    std::vector<uint32_t> mUIIndexBuffer;
    WindowHandle mWindowHandle;

    int mFrameIndex = -1;
    uint32_t mFenceValue = 0;
    bool mLastFrameContextSet = false;
    int mBackbufferIndex = -1;
};

class RENDER_FRONTEND_API RenderContextPool
{
  public:
    RenderContextPool();
    ~RenderContextPool();

    void Initalize(int poolSize = 20);
    RenderContext *Alloc();
    void Free(RenderContext *context);

    void DestoryPool();

  private:
    std::vector<RenderContext *> mRenderContextVec;
    std::mutex mMutex;
};

class RENDER_FRONTEND_API RenderContextSetPool
{
  public:
    void Initialize(int poolSize = 5);
    RenderContextSet *Alloc();
    void Free(RenderContextSet *contextSet);

  private:
    std::vector<RenderContextSet *> mRenderContextSetVec;
    std::mutex mMutex;
};

class RENDER_FRONTEND_API RenderPipelineManager
{
  public:
    static RenderPipelineManager *GetInstance();

    RenderPipelineManager();
    ~RenderPipelineManager();

    RenderPipelineManager(const RenderPipelineManager &rhs) = delete;
    RenderPipelineManager &operator=(const RenderPipelineManager &) = delete;

    void Update(uint64_t currentFrame, uint64_t completedFenceValue);

    void Execute(const std::vector<Core::LogicalWindow *> &logicalWindowList, WindowHandle windowHandle, int FrameIndex,
                 uint32_t mFrameFenceValue, bool lastExecute, int backBufferIndex, uint64_t frameTotalCount);

    void EndFrame();

    // backbuffer index return
    int WindowResize(void *windowHandle);

    void EndSystem();

#pragma region Thread

    // 사용할 수 있는context를 리턴
    RenderContext *GetFreeRenderContext();

    // 반환
    void ReturnRenderContext(RenderContext *renderContext);

    void EndRenderThread();

#pragma endregion
    RenderResourcePool mRenderResourcePool;

  private:
    void InitRenderGraph(Core::LogicalWindow *window, RenderContext *renderContext);
    // 매프레임 호출
    void BuildPassGraph(Core::LogicalWindow *window, RenderContext *renderContext);

    // 외부리소스를 등록
    void ImportResource(Core::LogicalWindow *window, RenderContext *renderContext, int backBufferIndex);

    void ExcuteRenderPassGraph(Core::LogicalWindow *window, RenderContext *renderContext);

    void CreateRenderCommands(World *world, RenderPassExecuteContext &executeContext);

    void BuildSkysphereSnapshot(World *world, RenderPassExecuteContext &executeContext);

  private:
    RenderContextPool mRenderContextPool;
    RenderContextSetPool mRenderContextSetPool;

    // RenderResourcePool

    // bool mRenderThreadRunning;
    RenderThread mRenderThread;
    bool mUseThread;
};

} // namespace Render