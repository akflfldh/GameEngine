#pragma once

#include <Core/LogicalWindow.h>
#include <Core/SuperController.h>
#include <Core/ViewportController.h>
#include <CoreBase/CallbackSystem.h>
#include <RenderSystem/RenderType.h>
#include <Window/IWindowEventHandler.h>
#include <Windows.h>

namespace Core
{
class LogicalWindow;
}

using OnMouseEnterWindowCallbackSystem = Core::MultiCallbackSystem<Core::LogicalWindow *>;

namespace UI
{
class UICanvas;
}

class ObjectHierarchyPanel;

namespace Quad
{
class BaseWindow;

class SuperFrameController : public Core::SuperController, public IWindowEventHandler
{
  public:
    static SuperFrameController *GetInstance();

    virtual ~SuperFrameController();

    void Initialize(Render::RenderPipelineManager &renderPipelineManager) override;

    virtual void SetWorkSpace(Core::WorkSpace *workspace);
    // void AddLogicalWindow(Core::LogicalWindow *window);
    void SetGlobalOverlayWindow(Core::LogicalWindow *window);

#pragma region WindowEventHandler

    virtual void OnInput() override;
    virtual void SetMousePos(int sx, int sy, int cx, int cy) override;
    virtual void OnMouseMove(int deltaX, int deltaY) override;
    virtual void OnMouseWheel(int wheelDelta) override;

    virtual void OnWindowMinimized() override;
    virtual void OnWindowRestored() override;
    virtual void OnWindowResize(unsigned int width, unsigned int height) override;
    virtual void OnMouseButtonEvent(EInputState mouseButtonState) override;
    virtual void OnKeyEvent(EInputState keyState, uint8_t scanKey) override;
    virtual void OnCharEvent(uint32_t ch) override;

#pragma endregion

    virtual void InitWorld() override;

    virtual void Begin() override;
    virtual void PreUpdate() override;

    virtual void Update(float deltaTime) override;

    virtual void EndUpdate() override;

    virtual void Draw(Render::RenderPipelineManager &renderPipelineManager) override;

    virtual void ShutDownWindow() override;

    virtual BaseWindow *GetWindow() const override;

    void TestLButtonDownCallback();

    // virtual void UpdateMouseInput(MouseContext &mouseContext) override;

    // 입력처리 InputSystem이 호출한다 .
    void OnInputEvent(const Quad::RawInputData &inputData);

    virtual void SetMouseCapture(Core::LogicalWindow *window) override;
    virtual void ReleaseMouseCapture() override;

    virtual void SetKeyboardCapture(Core::LogicalWindow *window) override;
    virtual void ReleaseKeyboardCapture() override;

    std::pair<uint32_t, uint32_t> GetWindowSize() const override;

    OnMouseEnterWindowCallbackSystem mOnMouseEnterWindowCallbackSystem;

  private:
    void OnResizeWindow(UINT clientWidth, UINT clientHeight);

    void InitViewportControllers();

    // 윈도우창의 크기가변할때 호출
    //    void ResizeViewportControllers(UINT width, UINT height);

    void InitMap();
    // map에 테스트용으로 object들을 생성
    void TestMap();

    // 현재 마우스가 위치한 논리적윈도우 반환
    Core::LogicalWindow *GetLogicalWindowOnMouse(const Quad::RawInputData &inputData);

    // void InitAssetBrowerWindow();
    // void InitGlobalOverlayWindow();

  private:
    SuperFrameController();
    BaseWindow *mWindow;

    UI::UICanvas *mUICanvas;
    //// 다른채널 (다른viewport를 가질것이다)
    UI::UICanvas *mUICanvas2;

    Core::LogicalWindow *mCurrentLogicalWindowOnMouse = nullptr;
    //   Core::LogicalWindow *mGlobalOverlayLogicalWindow = nullptr;

    Core::LogicalWindow *mMouseCapturedLogicalWindow = nullptr;
    Core::LogicalWindow *mKeyboardCapturedLogicalWindow = nullptr;

    ObjectHierarchyPanel *mObjectListPanel = nullptr;

    bool mRenderStop;
    bool mIsMinimized = false;
};

} // namespace Quad
