#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <Core/ViewportController.h>
#include <RenderSystem/RenderType.h>
namespace UI
{
class UICanvas;
}

namespace Core
{

// 논리적 윈도우 ( 실제 윈도우(controller)에 포함되는)
// viewport, renderchannel을 가진다.
// 또한 ui canvas list를 유지한다.

class CORE_API_LIB LogicalWindow
{
  public:
    LogicalWindow();
    ~LogicalWindow();

    void AddUICanvas(UI::UICanvas *uiCanvas);

    ViewportController mViewportController;
    LogicalWindowInputContext mLogicalWindowInputContext;

    Render::RenderChannelID GetRenderChannelID() const;
    void SetRenderChannelID(Render::RenderChannelID id);
    UI::UICanvas **GetUICanvasList() const;
    size_t GetUICanvasNum() const;

  private:
    UI::UICanvas **mUICanvasList;
    size_t mUICanvasListMax;
    size_t mCurrUICanvasNum;
    Render::RenderChannelID mRenderChannelID;
};

} // namespace Core