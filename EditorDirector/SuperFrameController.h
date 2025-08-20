#pragma once

#include "EditorDirector/SuperController.h"
#include <Core/LogicalWindow.h>
#include <Core/ViewportController.h>
#include <RenderSystem/RenderType.h>
#include <Windows.h>
namespace UI
{
class UICanvas;
}

namespace Quad
{
class BaseWindow;

class SuperFrameController : public SuperController
{
  public:
    static SuperFrameController *GetInstance();

    virtual ~SuperFrameController();

    void Initialize() override;

    virtual void PreUpdate() override;

    virtual void Update(float deltaTime) override;

    virtual void EndUpdate() override;

    virtual void Draw() override;

    virtual BaseWindow *GetWindow() override;

    void TestLButtonDownCallback();

    virtual void UpdateMouseInput(MouseContext &mouseContext) override;

  private:
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void OnResizeWindow(UINT clientWidth, UINT clientHeight);

    void InitViewportControllers();

    // 윈도우창의 크기가변할때 호출
    void ResizeViewportControllers(UINT width, UINT height);



  private:
    SuperFrameController() = default;
    BaseWindow *mWindow;

    UI::UICanvas *mUICanvas;
    //// 다른채널 (다른viewport를 가질것이다)
    UI::UICanvas *mUICanvas2;

    // Render::RenderChannelID mImGuiRenderChannelID;
    // Render::RenderChannelID mSubRenderChannelID;

    // Core::ViewportController mImGuiChannelViewportController;
    // Core::ViewportController mSubChannelViewportController;

    Core::LogicalWindow mImGuiLogicalWindow;
    Core::LogicalWindow mSubLogicalWindow;
};

} // namespace Quad
