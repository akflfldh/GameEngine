#pragma once

#include <CoreDevice/CommandContext.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace Render
{
struct RenderContextSet;
class RenderContextPool;
class RenderContextSetPool;

class RENDER_FRONTEND_API RenderThread
{
  public:
    RenderThread();
    ~RenderThread();

    void Initialize(RenderContextPool *renderContextPool, RenderContextSetPool *renderContextSetPool);

    void ThreadLoop();

    void PushRenderContextSet(RenderContextSet *contextSet);

    void EndThread();

    void ResetFrame(int frameIndex);

    void FlushAndStop(void *windowHandle);

  private:
    void Free(RenderContextSet *set);

  private:
    std::thread mThread;
    std::mutex mRenderContextQueueMutex;
    std::condition_variable mCV_Render;

    bool mRenderThreadRunning;

    std::queue<RenderContextSet *> mRenderContextSetQueue;

    RenderContextPool *mRenderContextPool;
    RenderContextSetPool *mRenderContextSetPool;

    std::unique_ptr<Core::CommandContext> mCommandContext;

    void *mStopWindowHandle;
    bool mFlushAndStopFlag;
    std::condition_variable mCV_FlushAndStop;
    std::mutex mFlushAndStopMutex;
};

} // namespace Render