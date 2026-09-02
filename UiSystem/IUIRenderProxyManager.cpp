#include "IUIRenderProxyManager.h"

UI::IUIRenderProxyManager *UI::IUIRenderProxyManager::mRenderProxyManager = nullptr;

UI::IUIRenderProxyManager *UI::IUIRenderProxyManager::GetInstance()
{

    if (mRenderProxyManager == nullptr)
        assert(0);

    return mRenderProxyManager;
}

void UI::IUIRenderProxyManager::SetRenderProxyManager(IUIRenderProxyManager *renderProxyManager)
{

    mRenderProxyManager = renderProxyManager;
}
