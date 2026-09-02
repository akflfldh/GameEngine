#include "WorkSpace.h"
#include <Core/LogicalWindow.h>

Core::WorkSpace::WorkSpace() {}

Core::WorkSpace::~WorkSpace() {}

void Core::WorkSpace::AddLogicalWindow(Core::LogicalWindow *window)
{

    if (window == nullptr)
        return;

    mWindowList.push_back(window);
}

void Core::WorkSpace::SetGlobalOverlayWindow(LogicalWindow *window)
{

    mGloberOverlayWindow = window;
}

void Core::WorkSpace::OnResizeWindow(uint32_t width, uint32_t height)
{

    for (size_t i = 0; i < mWindowList.size(); ++i)
    {
        mWindowList[i]->OnResizeWindow(width, height);
    }
}

void Core::WorkSpace::UpdateFrameData()
{

    for (size_t i = 0; i < mWindowList.size(); ++i)
    {
        mWindowList[i]->UpdateFrameData();
    }
}

Core::LogicalWindow *Core::WorkSpace::FindLogicalWindowAtClientPos(int x, int y) const
{

    for (auto logicalWindow : mWindowList)
    {
        if (logicalWindow == nullptr)
            continue;

        if (logicalWindow->mViewportController.IntersectPoint(x, y))
        {
            return logicalWindow;
        }
    }

    return nullptr;
}
