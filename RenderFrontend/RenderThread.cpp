#include "RenderThread.h"
#include <CoreDevice/CoreDevice.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IRenderSystem.h>

#ifdef D3DX

#include <CoreDevice/D3DCommandContext.h>

#endif

Render::RenderThread::RenderThread() : mFlushAndStopFlag(false) {}

Render::RenderThread::~RenderThread() {}

void Render::RenderThread::Initialize(RenderContextPool *renderContextPool, RenderContextSetPool *renderContextSetPool)
{
#ifdef D3DX

    mCommandContext = std::make_unique<Core::D3DCommandContext>();
    mCommandContext->Initialize();
#endif

    mRenderContextPool = renderContextPool;
    mRenderContextSetPool = renderContextSetPool;
    mRenderThreadRunning = true;

    mThread = std::thread(&Render::RenderThread::ThreadLoop, this);
}

void Render::RenderThread::ThreadLoop()
{

    RenderContextSet *renderContextSet = nullptr;

    uint32_t lastFenceValue = 0;
    while (1)
    {

        renderContextSet = nullptr;

        {
            std::unique_lock lock(mRenderContextQueueMutex);

            mCV_Render.wait(lock, [this]()
                            { return !mRenderThreadRunning || !mRenderContextSetQueue.empty() || mFlushAndStopFlag; });

            // RenderContextQueue 체크는 안할수도있을거같다.
            if (!mRenderThreadRunning && mRenderContextSetQueue.empty())
            {
                // 종료요청이왔다.
                break;
            }

            if (mFlushAndStopFlag)
            {
                std::queue<Render::RenderContextSet *> tempQueue;

                while (mRenderContextSetQueue.empty() == false)
                {
                    RenderContextSet *contextSet = mRenderContextSetQueue.front();
                    mRenderContextSetQueue.pop();

                    if (contextSet->mWindowHandle != mStopWindowHandle)
                    {
                        tempQueue.push(contextSet);
                    }
                    else
                    {

                        Free(contextSet);
                    }
                }

                mRenderContextSetQueue = std::move(tempQueue);
                renderContextSet = nullptr;

                lock.unlock();
                // Core::CoreDevice::GetInstance()->WaitFenceValue(lastFenceValue);
                // std::unique_lock lock(mFlushAndStopMutex);
                Core::CoreDevice::GetInstance()->FlushGpu(); // 또는 WaitIdle()

                lock.lock();

                mFlushAndStopFlag = false;
                mCV_FlushAndStop.notify_one();
            }
            else
            {
                renderContextSet = mRenderContextSetQueue.front();
                mRenderContextSetQueue.pop();
            }
        }

        // 실행

        if (renderContextSet != nullptr)
        {

            mCommandContext->Reset(renderContextSet->mFrameIndex);
            UIRenderItemBuilder::GetInstance()->ResetBufferOffset(renderContextSet->mFrameIndex);
            // IRenderSystem->SetCommandContext();
            IRenderSystem::GetInstance()->StartWindow(mCommandContext.get(), renderContextSet->mWindowHandle,
                                                      renderContextSet->mBackbufferIndex);

            for (int i = 0; i < renderContextSet->renderContextList.size(); ++i)
            {
                RenderContext *renderContext = renderContextSet->renderContextList[i];
                renderContext->mRenderPassExecuteContext.mCommandContext = mCommandContext.get();

                renderContext->mRenderPassGraph->Execute(renderContext->mRenderPassExecuteContext);
            }

            IRenderSystem::GetInstance()->PresentWindow(mCommandContext.get(), renderContextSet->mWindowHandle,
                                                        renderContextSet->mBackbufferIndex);

            lastFenceValue = renderContextSet->mFenceValue;

            // 프레임의 마지막 ContextSet이다.
            if (renderContextSet->mLastFrameContextSet)
            {
                // Fence설정

                Core::CoreDevice::GetInstance()->SetFence(lastFenceValue);
                GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
                if (gpuBufferContextSystem != nullptr)
                    gpuBufferContextSystem->PreUpdate();
            }

            Free(renderContextSet);
        }
    }
}

void Render::RenderThread::PushRenderContextSet(RenderContextSet *contextSet)
{

    std::unique_lock lock(mRenderContextQueueMutex);

    mRenderContextSetQueue.push(contextSet);

    mCV_Render.notify_one();
}

void Render::RenderThread::EndThread()
{

    mRenderThreadRunning = false;

    mCV_Render.notify_one();

    if (mThread.joinable())
    {
        mThread.join();
    }
}

void Render::RenderThread::ResetFrame(int frameIndex)
{

    mCommandContext->Reset(frameIndex);
}

void Render::RenderThread::FlushAndStop(void *windowHandle)
{

    std::unique_lock lock(mRenderContextQueueMutex);
    mStopWindowHandle = windowHandle;
    mFlushAndStopFlag = true;

    // 메인스레드가 진행
    // 메인스레드는 기다려야함

    mCV_Render.notify_one();

    mCV_FlushAndStop.wait(lock, [this]() { return !mFlushAndStopFlag; });
}

void Render::RenderThread::Free(RenderContextSet *set)
{

    for (int i = 0; i < set->renderContextList.size(); ++i)
    {
        RenderContext *renderContext = set->renderContextList[i];
        mRenderContextPool->Free(renderContext);
    }

    mRenderContextSetPool->Free(set);
}