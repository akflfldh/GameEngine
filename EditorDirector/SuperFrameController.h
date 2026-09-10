#pragma once

#include <Core/LogicalWindow.h>
#include <Core/ViewportController.h>
#include <Core/WindowedFrameController.h>
#include <CoreBase/CallbackSystem.h>
#include <RenderSystem/RenderType.h>
#include <Window/IWindowEventHandler.h>
#include <Windows.h>

namespace Core
{
class LogicalWindow;
}

namespace UI
{
class UICanvas;
}

class ObjectHierarchyPanel;

namespace Quad
{
class BaseWindow;

class SuperFrameController : public Core::WindowedFrameController
{
  public:
    static SuperFrameController *GetInstance();

    virtual ~SuperFrameController();

    void Initialize(Render::RenderPipelineManager &renderPipelineManager);

    // void AddLogicalWindow(Core::LogicalWindow *window);

    virtual void Begin() override;
    virtual void PreUpdate() override;

    virtual void Update(float deltaTime) override;

    virtual void EndUpdate() override;

    //  void TestLButtonDownCallback();

    // virtual void UpdateMouseInput(MouseContext &mouseContext) override;

    // 입력처리 InputSystem이 호출한다 .
    // void OnInputEvent(const Quad::RawInputData &inputData);

    //  virtual void SetMouseCapture(Core::LogicalWindow *window) override;
    //   virtual void ReleaseMouseCapture() override;

    //  virtual void SetKeyboardCapture(Core::LogicalWindow *window) override;
    //  virtual void ReleaseKeyboardCapture() override;

    // std::pair<uint32_t, uint32_t> GetWindowSize() const override;

    OnMouseEnterWindowCallbackSystem mOnMouseEnterWindowCallbackSystem;

  protected:
    virtual void OnRenderSurfaceResize(uint32_t width, uint32_t height) override;
    virtual void OnMouseDownOutsideOverlay() override;

  private:
    // void OnResizeWindow(UINT clientWidth, UINT clientHeight);

    // void InitViewportControllers();

    // 윈도우창의 크기가변할때 호출
    //    void ResizeViewportControllers(UINT width, UINT height);

    //  void InitMap();
    // map에 테스트용으로 object들을 생성
    //  void TestMap();

    // 현재 마우스가 위치한 논리적윈도우 반환
    //  Core::LogicalWindow *GetLogicalWindowOnMouse(const Quad::RawInputData &inputData);

    // void InitAssetBrowerWindow();
    // void InitGlobalOverlayWindow();
    virtual void DrawWindow() override;

  private:
    SuperFrameController();
    Render::RenderPipelineManager *mRenderPipelineManager = nullptr;
    //  UI::UICanvas *mUICanvas;
    //// 다른채널 (다른viewport를 가질것이다)
    //   UI::UICanvas *mUICanvas2;

    //   Core::LogicalWindow *mCurrentLogicalWindowOnMouse = nullptr;
    //   Core::LogicalWindow *mGlobalOverlayLogicalWindow = nullptr;

    //   Core::LogicalWindow *mMouseCapturedLogicalWindow = nullptr;
    //   Core::LogicalWindow *mKeyboardCapturedLogicalWindow = nullptr;

    //   ObjectHierarchyPanel *mObjectListPanel = nullptr;

    //   bool mRenderStop;
    //   bool mIsMinimized = false;
};

} // namespace Quad
