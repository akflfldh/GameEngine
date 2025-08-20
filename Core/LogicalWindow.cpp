#include "LogicalWindow.h"

Core::LogicalWindow::LogicalWindow() : mUICanvasList(nullptr),mCurrUICanvasNum(0)
{
    mUICanvasList = new UI::UICanvas *[5];
}

Core::LogicalWindow::~LogicalWindow()
{
    delete[] mUICanvasList;
}

void Core::LogicalWindow::AddUICanvas(UI::UICanvas *uiCanvas)
{

    if (uiCanvas == nullptr)
        return;

    if (mCurrUICanvasNum > mUICanvasListMax)
    {
        // 재할당
    }

    mUICanvasList[mCurrUICanvasNum] = uiCanvas;

    mCurrUICanvasNum++;
}

Render::RenderChannelID Core::LogicalWindow::GetRenderChannelID() const
{
    return mRenderChannelID;
}

void Core::LogicalWindow::SetRenderChannelID(Render::RenderChannelID id)
{

    mRenderChannelID = id;
}

UI::UICanvas **Core::LogicalWindow::GetUICanvasList() const
{
    return mUICanvasList;
}

size_t Core::LogicalWindow::GetUICanvasNum() const
{
    return mCurrUICanvasNum;
}
