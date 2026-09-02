#include "IRenderProxyManager.h"
#include <CoreBase/CoreAssert.h>
Core::IRenderProxyManager *Core::IRenderProxyManager::mRenderProxyManager = nullptr;

Core::IRenderProxyManager *Core::IRenderProxyManager::GetInstance()
{

    CHECK(mRenderProxyManager != nullptr);

    return mRenderProxyManager;
}

void Core::IRenderProxyManager::SetRenderProxyManager(IRenderProxyManager *renderProxyManager)
{
    mRenderProxyManager = renderProxyManager;
}
