#pragma once

#include <Core/SuperController.h>
#include <Window/IWindowEventHandler.h>
#include <memory>

namespace Quad
{
class BaseWindow;
}

namespace UI
{
class UICanvas;
}
using OnMouseEnterWindowCallbackSystem = Core::MultiCallbackSystem<Core::LogicalWindow *>;

namespace Core
{
struct WindowCreateDesc
{
    std::string mWindowClassName;
    std::string mWindowName;
    uint32_t mWindowStyle;
    uint32_t mWindowClassStyle;

    uint32_t mMaxClientWidth;
    uint32_t mMaxClientHeight;
    uint32_t mClientWidth;
    uint32_t mClientHeight;
};

class CORE_API_LIB WindowedFrameController : public SuperController, public IWindowEventHandler
{
  public:
    WindowedFrameController();
    virtual ~WindowedFrameController();

    virtual void Begin() override;

    virtual void PreUpdate() override;

    virtual void Update(float deltaTime) override;

    // 여기서는 렌더링 위한 사전작업등을한다(렌더아이템구축,제출)
    virtual void EndUpdate() override;

    virtual void Draw();

    virtual void ShutDownWindow();
    // virtual void SetWorkSpace(Core::WorkSpace *workspace);
#pragma region input

    virtual void OnInput() override;
    virtual void SetMousePos(int sx, int sy, int cx, int cy) override;
    virtual void OnMouseMove(int deltaX, int deltaY) override;
    virtual void OnMouseWheel(int wheelDelta) override;
    virtual void OnMouseButtonEvent(EInputState mouseButtonState) override;
    virtual void OnKeyEvent(EInputState keyState, uint8_t scanKey) override;
    virtual void OnCharEvent(uint32_t ch) override;

    virtual void OnWindowMinimized() override;
    void OnWindowRestored() override;
    virtual void OnWindowResize(unsigned int width, unsigned int height) override;

#pragma endregion
    void OnInputEvent(const Quad::RawInputData &inputData);

    // virtual void UpdateMouseInput(Quad::MouseContext &mouseContext) = 0;

    Quad::BaseWindow *GetWindow() const;

    // World *GetWorld() const;

    virtual void SetMouseCapture(Core::LogicalWindow *window);
    virtual void ReleaseMouseCapture();

    virtual void SetKeyboardCapture(Core::LogicalWindow *window);
    virtual void ReleaseKeyboardCapture();

    std::pair<uint32_t, uint32_t> GetWindowSize() const;

    virtual Core::WorkSpace *GetWorkSpace() const;
    virtual void SetWorkSpace(Core::WorkSpace *workspace);

    OnMouseEnterWindowCallbackSystem mOnMouseEnterWindowCallbackSystem;

    void ShowWindow();
    void MaximizedWindow();

  protected:
    void InitializeWindow(const WindowCreateDesc &creationDesc);

    virtual void OnRenderSurfaceResize(uint32_t width, uint32_t height);
    Core::LogicalWindow *GetLogicalWindowOnMouse(const Quad::RawInputData &inputData);

    virtual void OnMouseDownOutsideOverlay();
    virtual void DrawWindow();

  private:
    std::unique_ptr<Quad::BaseWindow> mWindow;
    WorkSpace *mWorkSpace = nullptr;
    Core::LogicalWindow *mCurrentLogicalWindowOnMouse = nullptr;
    //   Core::LogicalWindow *mGlobalOverlayLogicalWindow = nullptr;

    Core::LogicalWindow *mMouseCapturedLogicalWindow = nullptr;
    Core::LogicalWindow *mKeyboardCapturedLogicalWindow = nullptr;

    bool mRenderStop = false;
    bool mIsMinimized = false;
};
} // namespace Core
